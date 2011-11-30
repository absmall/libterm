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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <bps/bps.h>
#ifdef BPS_VERSION
#include <bps/virtualkeyboard.h>
#else
#include <bbsupport/Keyboard>
#include <bbsupport/Notification>
#endif
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
    piekey_active = 0;
    piekeyboard = new QPieKeyboard(this);
    piekeyboard->initialize( 6, "abcdefghijklmnopqrstuvwxyz0123456789" );
#ifndef __QNX__
    piekeyboard->testMode(3);
#endif

#ifdef __QNX__
    resize(1024, 600);
#endif
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
#ifdef __QNX__
//    BlackBerry::Keyboard::instance().show();
#endif
#endif
    cursor_timer->start(BLINK_SPEED);
    setAttribute(Qt::WA_AcceptTouchEvents);
}

QTerm::~QTerm()
{
    delete notifier;
    delete exit_notifier;
    delete piekeyboard;
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

void QTerm::piekeypress(char key)
{
    term_send_data( terminal, &key, 1 );
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
    cursor_on ^= 1;
    update( cursor_x * char_width,
                  cursor_y * char_height,
                  char_width, char_height );
}

void QTerm::paintEvent(QPaintEvent *event)
{
    int i, j, color;
    int new_width;
    int new_height;
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
    
    new_width = painter.fontMetrics().maxWidth();
    // Workaround for a bug in OSX - Dave reports that maxWidth returns 0,
    // when width of different characters returns the correct value
    if( new_width == 0 ) {
        new_width = painter.fontMetrics().width(QChar('X'));
    }
    new_height = painter.fontMetrics().lineSpacing();
    
    if( char_width != new_width
     || char_height != new_height ) {
        char_width = new_width;
        char_height = new_height;
        char_descent = painter.fontMetrics().descent();
#ifdef __QNX__
        {
            int kbd_height;
#ifdef BPS_VERSION
            virtualkeyboard_get_height( &kbd_height );
#else
            kbd_height = BlackBerry::Keyboard::instance().keyboardHeight();
#endif
            term_resize( terminal, contentsRect().width() / char_width, (contentsRect().height() - kbd_height) / char_height, 0 );
        }
#else
        term_resize( terminal, contentsRect().width() / char_width, contentsRect().height() / char_height, 0 );
#endif
        update( contentsRect() );
        return;
    }

    painter.setPen(QColor(255, 255, 255));
    painter.setBrush(QColor(255, 255, 255));
    grid = term_get_grid( terminal );
    attribs = term_get_attribs( terminal );
    colors = term_get_colours( terminal );
    for( i = 0; i < term_get_height( terminal ); i ++ ) {
        for( j = 0; j < term_get_width( terminal ); j ++ ) {
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

void QTerm::mousePressEvent(QMouseEvent *event)
{
#ifndef __QNX__
    piekeyboard->activate(event->x(), event->y(), event->x()+10, event->y()+10);
#endif
}

void QTerm::resizeEvent(QResizeEvent *event)
{
    if( char_width != 0 && char_height != 0 ) {
#ifdef __QNX__
        {
            int kbd_height;
#ifdef BPS_VERSION
            virtualkeyboard_get_height( &kbd_height );
#else
            kbd_height = BlackBerry::Keyboard::instance().keyboardHeight();
#endif
            term_resize( terminal, event->size().width() / char_width, (event->size().height() - kbd_height) / char_height, 0 );
        }
#else
        term_resize( terminal, event->size().width() / char_width, event->size().height() / char_height, 0 );
#endif
    }
}

bool QTerm::event(QEvent *event)
{
    QList<QTouchEvent::TouchPoint> touchPoints;

    switch(event->type()) {
        case QEvent::TouchBegin:
        case QEvent::TouchUpdate:
        case QEvent::TouchEnd:
            touchPoints = static_cast<QTouchEvent *>(event)->touchPoints();

            switch(event->type()) {
                case QEvent::TouchBegin:
                    return true;
                case QEvent::TouchUpdate:
                    if( touchPoints.length() >= 2 ) {
                        if( !piekey_active ) {
                            piekey_active = 1;
                            piekeyboard->activate(touchPoints[0].pos().x(), touchPoints[0].pos().y(),
                                                  touchPoints[1].pos().x(), touchPoints[1].pos().y() );
                        }
                        piekeyboard->moveTouch(0, touchPoints[0].pos().x(), touchPoints[0].pos().y());
                        piekeyboard->moveTouch(1, touchPoints[1].pos().x(), touchPoints[1].pos().y());
                    } else {
                        if( piekey_active ) {
                            piekeyboard->release();
                            piekey_active = 0;
                        }
                    }
                    return true;
                case QEvent::TouchEnd:
                    piekeyboard->release();
                    piekey_active = 0;
                    return true;
                default:
                    break;
            }
            break;
        default:
            return QWidget::event(event);
    }

    return false;
}

int main(int argc, char *argv[])
{
    term_t terminal;

#ifdef __QNX__
#ifdef BPS_VERSION
    if( bps_initialize() != BPS_SUCCESS ) {
        fprintf(stderr, "Failed to initialize bps (%s)\n", strerror( errno ) );
        exit(1);
    }
#endif
#endif

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
