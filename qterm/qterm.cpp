#include <QPainter>
#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QPaintEvent>
#include <QFontMetrics>
#include <QTextStream>
#include <qterm.h>
#include <stdio.h>
#include <QKeyEvent>
#include <QTimer>
#include <sys/select.h>
#include <errno.h>
#ifdef __QNX__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <bbsupport/Keyboard>
#include <bbsupport/Notification>
#endif

#define WIDTH    80
#define HEIGHT    17
#define BLINK_SPEED 1000

QTerm::QTerm(QWidget *parent) : QWidget(parent)
{
    term_create( &terminal );
    term_begin( terminal, WIDTH, HEIGHT, 0 );
    init();
}

QTerm::QTerm(QWidget *parent, term_t terminal) : QWidget(parent)
{
    this->terminal = terminal;
    init();
}

void QTerm::init()
{
    char_width = 0;
    char_height = 0;
    cursor_x = -1;
    cursor_y = -1;
    cursor_on = 1;

    term_set_user_data( terminal, this );
    term_register_update( terminal, term_update );
    term_register_cursor( terminal, term_update_cursor );
    term_register_bell( terminal, term_bell );
    notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );
    exit_notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Exception );
    cursor_timer = new QTimer( this );
    QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
    QObject::connect(exit_notifier, SIGNAL(activated(int)), this, SLOT(terminate()));
    QObject::connect(cursor_timer, SIGNAL(timeout()), this, SLOT(blink_cursor()));

    // Setup the initial font
    font = new QFont();
    font->setStyleHint(QFont::Courier);//QFont::TypeWriter);
    font->setPointSize(12);
    font->setStyleStrategy(QFont::NoAntialias);
    font->setFamily("Monospace");
    font->setFixedPitch(true);
    font->setKerning(false);

    // Workaround for a bug in OSX - Dave reports that maxWidth returns 0,
    // when width of different characters returns the correct value
    QFontMetrics metrics(*font);
    char_width = metrics.maxWidth();
    if(char_width==0) {
        fontWorkAround = true;
        char_width = metrics.width(QChar('X'));
    } else {
        fontWorkAround = false;
    }

    char_height = metrics.lineSpacing();
    char_descent = metrics.descent();

#ifdef __QNX__
    BlackBerry::Keyboard::instance().show();
#endif
    cursor_timer->start(BLINK_SPEED);
}

QTerm::~QTerm()
{
    delete notifier;
    delete exit_notifier;
    term_free( terminal );
}

void QTerm::term_bell(term_t handle)
{
#ifdef __QNX__
    char command[] = "msg::play_sound\ndat:json:{\"sound\":\"notification_general\"}";
    int f = open("/pps/services/multimedia/sound/control", O_RDWR);
    write(f, command, sizeof(command));
    ::close(f);
#else
    QApplication::beep();
#endif
}

void QTerm::term_update(term_t handle, int x, int y, int width, int height)
{
    QTerm *term = (QTerm *)term_get_user_data( handle );

    term->update_grid(x, y, width, height);
}

void QTerm::term_update_cursor(term_t handle, int x, int y)
{
    QTerm *term = (QTerm *)term_get_user_data( handle );
    // Update old cursor location
    term->cursor_on = 0;
    term->update_grid( term->cursor_x, term->cursor_y, 1, 1);

    // TODO:  Should also reset the blink timer to have consistency.
    term->cursor_on = 1;
    term->cursor_x = x;
    term->cursor_y = y;
    
    term->update_grid( term->cursor_x, term->cursor_y, 1, 1);
}

void QTerm::terminal_data()
{
    if( !term_process_child( terminal ) ) {
        exit(0);
    }
}

void QTerm::terminate()
{
    exit(0);
}

void QTerm::blink_cursor()
{
    //int coord_x;
    cursor_on ^= 1;
    update_grid( cursor_x, cursor_y, 1, 1);
}

// Called to update the grid.
// Region is based on grid coordinates.
void QTerm::update_grid(int grid_x_min,
                        int grid_y_min,
                        int grid_width,
                        int grid_height)
{
    int coords_x_min, coords_y_min;
    int coords_x_max, coords_y_max;

//    fprintf(stderr,"Updating Grid: (%d,%d) %d x %d\n", grid_x_min, grid_y_min, grid_width, grid_height);
    
    if (grid_x_min < 0 || grid_y_min < 0) {
        return;
    }
    if ( fontWorkAround ) {
        // If fontWorkAround is set, we cannot trust the characters to 
        // be monospaced.  So we need to calculate the coordinates based
        // on the string lengths of the strings in the grid.
        QFontMetrics metrics(*font);

        const char *str;
        int i;

        coords_x_min = 9999;    // Replace with some window property.
        coords_x_max = 0;
        for (i= grid_y_min; i < (grid_y_min + grid_height); i++) {
            int tmp;
            str = term_get_line( terminal, i );
                
            // X limits
            tmp = metrics.width(QString(str),grid_x_min);
            if (tmp < coords_x_min) {
                coords_x_min = tmp;
            }
            tmp = metrics.width(QString(str),grid_x_min + grid_width);
            if (tmp > coords_x_max) {
                coords_x_max = tmp;
            }
        }
        
    } else {
        coords_x_min = grid_x_min * char_width;
        coords_x_max = grid_width * char_width;
    }
    coords_y_min = grid_y_min * char_height;
    coords_y_max = grid_height * char_height;
    QWidget::update(coords_x_min, coords_y_min,
                    coords_x_max, coords_y_max);
}
void QTerm::paintEvent(QPaintEvent *event)
{
    int i;
#if 0
    int color;
    const wchar_t **grid;
    const uint32_t **attribs;
    const uint32_t **colors;
#endif
    QPainter painter(this);
    QRect windowRect = painter.window();
    const char *str;
    int cursor_x_coord;
    QColor fgColor(255,255,255);
    QColor bgColor(0,0,0);
    int gridWidth, gridHeight;

    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBrush(QColor(8, 0, 0));
    painter.setFont( *font );

    // Get grid dimensions
    term_get_grid_size(terminal, &gridWidth, &gridHeight);

    // First erase the grid with its current dimensions
    painter.drawRect(event->rect());
   
//    fprintf(stderr,"Rect: (%d, %d) %d x %d\n", event->rect().x(), event->rect().y(), event->rect().width(), event->rect().height());
   
    if (cursor_x < 0 || cursor_y < 0) {
        return;
    }

#if 1 
    str = term_get_line( terminal, cursor_y );
    // Workaround to get the cursor in the right spot.  For some
    // reason, on OSX (again), the monospace font does is not really
    // monospace for skinny characters! 
    if (fontWorkAround) {
        cursor_x_coord = painter.fontMetrics().width(QString(str),cursor_x);
    } else {
        cursor_x_coord = cursor_x * char_width;
    }

    if ( cursor_on ) {
       painter.setPen(fgColor);
       painter.setBrush(fgColor);
       painter.drawRect( cursor_x_coord +1, 
                         cursor_y * char_height + 1,
                         char_width-2, char_height-2); 
    } else {
       painter.setPen(bgColor);
       painter.setBrush(bgColor);
       painter.drawRect( cursor_x_coord +1,
                         cursor_y * char_height + 1,
                         char_width-2, char_height-2); 
    }
    painter.setPen(fgColor);
    painter.setBrush(fgColor);

    for (i=0; i< gridHeight;i++) {
        str = term_get_line( terminal, i );
        painter.drawText(0, (i) * char_height, 
                        windowRect.width(),char_height,
                        Qt::TextExpandTabs, 
                        str,
                        &windowRect 
                        );
    }

#else
    grid = term_get_grid( terminal );
    attribs = term_get_attribs( terminal );
    colors = term_get_colours( terminal );

    painter.setBrush(fgColor);
    for( i = 0; i < HEIGHT; i ++ ) {
        for( j = 0; j < WIDTH; j ++ ) {
            if( cursor_on && j == cursor_x && i == cursor_y ) {
                painter.drawRect(j * char_width + 1, i * char_height + 1, char_width - 2, char_height - 2);
                painter.setPen(QColor(0, 0, 0));
                painter.drawText(j * char_width, (i + 1) * char_height - char_descent, QString( QChar( grid[ i ][ j ] ) ) );
                painter.setPen(QColor(255, 255, 255));
            } else {

                color = term_get_fg_color( attribs[ i ][ j ], colors[ i ][ j ] );
                painter.setPen(QColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
                painter.drawText(j * char_width, (i + 1) * char_height - char_descent, QString( QChar( grid[ i ][ j ] ) ) );
            }
        }
    }
#endif
}

void QTerm::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        // FIXME These first two are a workaround for a bug in QT. Remove once it is fixed
        case Qt::Key_CapsLock:
        case Qt::Key_Shift:
            break;
        case Qt::Key_Up:
            term_send_special( terminal, TERM_KEY_UP );
            break;
        case Qt::Key_Down:
            term_send_special( terminal, TERM_KEY_DOWN );
            break;
        case Qt::Key_Right:
            term_send_special( terminal, TERM_KEY_RIGHT );
            break;
        case Qt::Key_Left:
            term_send_special( terminal, TERM_KEY_LEFT );
            break;
        default:
            term_send_data( terminal, event->text().toUtf8().constData(), event->text().count() );
            break;
    }
}
 
int main(int argc, char *argv[])
{
    term_t terminal;
    if( !term_create( &terminal ) ) {
        fprintf(stderr, "Failed to create terminal (%s)\n", strerror( errno ) );
        exit(1);
    }
    if( !term_begin( terminal, WIDTH, HEIGHT, 0 ) ) {
        fprintf(stderr, "Failed to begin terminal (%s)\n", strerror( errno ) );
        exit(1);
    }
    {
        QCoreApplication::addLibraryPath("app/native/lib");
        QApplication app(argc, argv);
     
        QTerm term(NULL, terminal);
        term.show();
 
        return app.exec();
    }
}
