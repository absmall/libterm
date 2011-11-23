#include <QPainter>
#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QPaintEvent>
#include <QFontMetrics>
#include <qterm.h>
#include <stdio.h>
#include <QKeyEvent>
#include <QTimer>
#include <sys/select.h>
#include <errno.h>
#ifdef __QNX__
#include <bbsupport/Keyboard>
#endif

#define WIDTH    80
#define HEIGHT    17

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
    notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );
    QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
#ifdef __QNX__
    BlackBerry::Keyboard::instance().show();
#endif
}

QTerm::~QTerm()
{
    delete notifier;
    term_free( terminal );
}

void QTerm::term_update(term_t handle, int x, int y, int width, int height)
{
    QTerm *term = (QTerm *)term_get_user_data( handle );

    term->update( x * term->char_width, y * term->char_height,
                  width * term->char_width, height * term->char_height + term->char_descent );
}

void QTerm::term_update_cursor(term_t handle, int x, int y)
{
    QTerm *term = (QTerm *)term_get_user_data( handle );

    // Update old cursor location
    term->update( term->cursor_x * term->char_width,
                  term->cursor_y * term->char_height,
                  term->char_width, term->char_height );

    term->cursor_x = x;
    term->cursor_y = y;

    // Update new cursor location
    term->update( term->cursor_x * term->char_width,
                  term->cursor_y * term->char_height,
                  term->char_width, term->char_height );
}

void QTerm::terminal_data()
{
    if( !term_process_child( terminal ) ) {
        exit(0);
    }
}

void QTerm::paintEvent(QPaintEvent *event)
{
    int i, j, color;
    const uint32_t **grid;
    const uint32_t **attribs;
    const uint32_t **colors;
    QPainter painter(this);
    QFont font;

    font.setStyleHint(QFont::TypeWriter);
    font.setFamily("Monospace");
    font.setFixedPitch(true);
    font.setKerning(false);
    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBrush(QColor(8, 0, 0));
    painter.setFont(font);

    // First erase the grid with its current dimensions
    painter.drawRect(event->rect());
    
    if( char_width != painter.fontMetrics().maxWidth()
     || char_height != painter.fontMetrics().lineSpacing() ) {
        char_width = painter.fontMetrics().maxWidth();
        char_height = painter.fontMetrics().lineSpacing();
        char_descent = painter.fontMetrics().descent();
        update( contentsRect() );
        return;
    }

    painter.setPen(QColor(255, 255, 255));
    painter.setBrush(QColor(255, 255, 255));
    grid = term_get_grid( terminal );
    attribs = term_get_attribs( terminal );
    colors = term_get_colours( terminal );
    for( i = 0; i < HEIGHT; i ++ ) {
        for( j = 0; j < WIDTH; j ++ ) {
            if( cursor_on && j == cursor_x && i == cursor_y ) {
                painter.drawRect(j * char_width + 1, i * char_height + 1, char_width - 2, char_height - 2);
                painter.setPen(QColor(0, 0, 0));
                painter.drawText(j * char_width, (i + 1) * char_height, QString( QChar( grid[ i ][ j ] ) ) );
                painter.setPen(QColor(255, 255, 255));
            } else {
                color = term_get_fg_color( attribs[ i ][ j ], colors[ i ][ j ] );
                painter.setPen(QColor((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF));
                painter.drawText(j * char_width, (i + 1) * char_height, QString( QChar( grid[ i ][ j ] ) ) );
            }
        }
    }
}

void QTerm::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        case Qt::Key_Return:
            term_send_data( terminal, "\n", 1 );
            break;
        case Qt::Key_Backspace:
            term_send_data( terminal, "\b", 1 );
            break;
        case Qt::Key_CapsLock:
        case Qt::Key_Shift:
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
