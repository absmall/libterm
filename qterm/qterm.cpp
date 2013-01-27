#include <QPainter>
#include <QAbstractEventDispatcher>
#include <QDesktopWidget>
#include <QApplication>
#include <QPaintEvent>
#include <QFontMetrics>
#include <QTextStream>
#include <qterm.h>
#include <stdio.h>
#include <QKeyEvent>
#include <QTimer>
#include <sys/select.h>
#include <pthread.h>
#include <errno.h>
#ifdef __QNX__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <bps/bps.h>
#include "logging.h"
#ifdef BPS_VERSION
#include <bps/virtualkeyboard.h>
#else
#include <bbsupport/Keyboard>
#include <bbsupport/Notification>
#endif
#endif
#include <dlfcn.h>

#define WIDTH    80
#define HEIGHT    17
#define BLINK_SPEED 1000

#define ON_CURSOR(x,y) (cursor_on && cursor_x == x && cursor_y == y)
QAbstractEventDispatcher::EventFilter QTerm::prevFilter;
QTerm *QTerm::instance = NULL;

QTerm::QTerm(QWidget *parent) : QWidget(parent)
{
    if( instance == NULL ) {
        instance = this;
    } else {
        throw "Singleton error";
    }
    init();
}

QTerm::QTerm(QWidget *parent, term_t terminal) : QWidget(parent)
{
    if( instance == NULL ) {
        instance = this;
    } else {
        throw "Singleton error";
    }
    this->terminal = terminal;
    init();
}

void QTerm::init()
{
    char_width = 0;
    char_height = 0;
    cursor_on = 1;
    piekey_active = 0;
    piekeyboard = new QPieKeyboard(this);
    piekeyboard->initialize( 6, "abcdefghijklmnopqrstuvwxyz|\n        " );
#ifndef __QNX__
    piekeyboard->testMode(3);
#endif

    term_set_user_data( terminal, this );
    term_register_update( terminal, term_update );
    term_register_cursor( terminal, term_update_cursor );
    term_register_bell( terminal, term_bell );
    notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );

#if !defined(__MACH__) && !defined(__QNX__)
    // Not supported on OSX
    exit_notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Exception );
    QObject::connect(exit_notifier, SIGNAL(activated(int)), this, SLOT(terminate()));
#endif

    cursor_timer = new QTimer( this );
    QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
    QObject::connect(cursor_timer, SIGNAL(timeout()), this, SLOT(blink_cursor()));

    // Setup the initial font

#ifdef __QNX__
    font = new QFont("Andale Mono");
#else
    font = new QFont();
    font->setStyleHint(QFont::Monospace);
    font->setStyleStrategy(QFont::NoAntialias);
    font->setFamily("Monospace");
    font->setFixedPitch(true);
    font->setKerning(false);
#endif
    if( QApplication::desktop()->screenGeometry().width() < 1000 ) {
        font->setPointSize(6);
    } else {
        font->setPointSize(12);
    }

    // Workaround for a bug in OSX - Dave reports that maxWidth returns 0,
    // when width of different characters returns the correct value
    QFontMetrics metrics(*font);
    if(metrics.maxWidth() == 0) {
        fontWorkAround = true;
    } else {
        fontWorkAround = false;
    }

    char_width = metrics.width(QChar(' '));
    char_height = metrics.lineSpacing();
    char_descent = metrics.descent();

    QObject::connect(piekeyboard, SIGNAL(keypress(char)), this, SLOT(piekeypress(char)));
#ifdef __QNX__
    showFullScreen();
    virtualkeyboard_request_events(0);
#endif
    cursor_timer->start(BLINK_SPEED);
    setAttribute(Qt::WA_AcceptTouchEvents);

    prevFilter = QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
    virtualkeyboard_show();
}

bool QTerm::eventFilter(void *message)
{
    bps_event_t * const event = static_cast<bps_event_t *>(message);
 
    if (event && bps_event_get_domain(event) == virtualkeyboard_get_domain()) {
        const int id = bps_event_get_code(event);
        switch( id ) {
            case VIRTUALKEYBOARD_EVENT_VISIBLE:
                instance->keyboardVisible = true;
                instance->resize_term();
                break;
            case VIRTUALKEYBOARD_EVENT_HIDDEN:
                instance->keyboardVisible = false;
                instance->resize_term();
                break;
            case VIRTUALKEYBOARD_EVENT_INFO:
                instance->resize_term();
                break;
            default:
                slog("Unexpected keyboard event %d", id);
                break;
        }
    }
 
    if (prevFilter)
        return prevFilter(message);
    else
        return false;
}

QTerm::~QTerm()
{
    delete notifier;
    delete exit_notifier;
    delete font;
    delete piekeyboard;
    if( instance == this ) {
        instance = NULL;
    }
    term_free( terminal );
}

#ifdef __QNX__
void QTerm::resize_term()
{
    int kbd_height;
    sleep(1);
    if( keyboardVisible ) {
        virtualkeyboard_get_height( &kbd_height );
    } else {
        kbd_height = 0;
    }
    slog("resize term! %d %d %d %d -> (%dx%d)", size().width(), size().height(), char_width, kbd_height, size().width()/char_width, (size().height() - kbd_height)/ char_height);
    term_resize( terminal, size().width() / char_width, (size().height() - kbd_height) / char_height, 0 );
    QWidget::update(0, 0,
                    size().width(), size().height());
}
#endif

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

void QTerm::term_update_cursor(term_t handle, int old_x, int old_y, int new_x, int new_y)
{
    QTerm *term = (QTerm *)term_get_user_data( handle );

    // Reset cursor blink
    term->cursor_on = 1;
    term->cursor_timer->stop();
    term->cursor_timer->start(BLINK_SPEED);

    // Update old and new cursor location
    term->update_grid( old_x, old_y, 1, 1);
    term->update_grid( new_x, new_y, 1, 1);
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
    int cursor_x, cursor_y;

    cursor_on ^= 1;
    term_get_cursor_pos( terminal, &cursor_x, &cursor_y );
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

        coords_x_min = width();
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

// Returns bool if the string, located at grid location X,Y, will be within
// the updateRect.
void QTerm::getRenderedStringRect( const QString string,
                                   int attrib,
                                   QFont *pFont,
                                   QRect *pUpdateRect) 
{
    QFontMetrics *pFontMetrics = NULL;
    QFont *pTmpFont;
    
    if (pFont == NULL) {
        pTmpFont = font;
    } else {
        pTmpFont = pFont;
    }

    pTmpFont->setUnderline( attrib & TERM_ATTRIB_UNDERSCORE );
    pFontMetrics = new QFontMetrics( *pTmpFont );

    pUpdateRect->setWidth( pFontMetrics->width(  string ));
    pUpdateRect->setHeight( char_height );
    
    delete pFontMetrics;

}

void QTerm::paintEvent(QPaintEvent *event)
{
    int i;
    const char *str;
    int cursor_x_coord;
    QPainter painter(this);
    QColor fgColor(255,255,255);
    QColor bgColor(0,0,0);
    int gridWidth, gridHeight;
    int cursor_x, cursor_y;
    const uint32_t **colors;
    const uint32_t **attribs;

    painter.setBackgroundMode(Qt::TransparentMode);
    painter.setBrush(QColor(8, 0, 0));
    painter.setFont( *font );

    // Get grid dimensions
    term_get_grid_size(terminal, &gridWidth, &gridHeight);

    // First erase the grid with its current dimensions
    painter.drawRect(event->rect());
   
    //log("Rect: (%d, %d) %d x %d\n", event->rect().x(), event->rect().y(), event->rect().width(), event->rect().height());

    painter.setPen(fgColor);
    painter.setBrush(fgColor);
    term_get_cursor_pos( terminal, &cursor_x, &cursor_y );

    attribs = term_get_attribs( terminal );
    colors = term_get_colours( terminal );

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
       painter.drawRect( cursor_x_coord + 1, 
                         cursor_y * char_height + 1,
                         char_width - 2, char_height - 2); 
    }
    painter.setPen(fgColor);
    painter.setBrush(fgColor);

        
    for (i=0; i< gridHeight;i++) {
        unsigned int currentAttrib; 
        unsigned int currentColor;
        bool currentOnCursor;
        int color=0x00ffffff;
        int chunkStart=0;
        int chunkPos;
        int last_x_pos = 0;
        bool recalcSubString = false;
        QString qString;
        QStringRef subString;
        QRect stringRect;
        QRect intersectedRect;
       
        currentAttrib = attribs[i][chunkStart];
        currentColor = colors[i][chunkStart];
        currentOnCursor = ON_CURSOR(0, i);
        color = term_get_fg_color(currentAttrib, currentColor);
        str = term_get_line( terminal, i );
        qString = qString.append(str);
        painter.setPen( QColor( (color >> 16) & 0xFF,
                                (color >> 8 ) & 0xFF,
                                (color & 0xFF) ) );

        stringRect.setX( 0 );
        stringRect.setY( i * char_height );
        getRenderedStringRect( qString, currentAttrib, NULL, &stringRect);

        intersectedRect = stringRect.intersected( event->rect() );

        if (intersectedRect.height() == 0) { 
            // Don't render this string as it is not in the event's height
            continue;
        }

        /* Chunk each string whenever we need to change rendering params */
        for(chunkPos=0; chunkPos< gridWidth; chunkPos++) {
            if( attribs[i][chunkPos] != currentAttrib ||
                colors[i][chunkPos] != currentColor ||
                ON_CURSOR(chunkPos, i) != currentOnCursor) {
                // flag to tell outer loop to recalc the substring
                recalcSubString = true;

                subString = qString.midRef( chunkStart, chunkPos-chunkStart );
                // Update the intersected rect with the substring
                getRenderedStringRect( subString.toString(), 
                                      currentAttrib, 
                                      NULL,
                                      &intersectedRect);

                // Render everything up to this point.
                if( currentOnCursor ) {
                    painter.setPen( bgColor );
                } else if( currentAttrib & TERM_ATTRIB_REVERSE ) {
                    color = term_get_fg_color( currentAttrib, currentColor );
                    painter.setPen( QColor( (color >> 16) & 0xFF,
                                            (color >> 8 ) & 0xFF,
                                            (color & 0xFF) ) );
                    painter.drawRect( last_x_pos, intersectedRect.y(),
                                      intersectedRect.width(), intersectedRect.height() );
                    color = term_get_bg_color( currentAttrib, currentColor );
                    painter.setPen( QColor( (color >> 16) & 0xFF,
                                            (color >> 8 ) & 0xFF,
                                            (color & 0xFF) ) );
                } else {
                    color = term_get_fg_color( currentAttrib, currentColor );
                    painter.setPen( QColor( (color >> 16) & 0xFF,
                                            (color >> 8 ) & 0xFF,
                                            (color & 0xFF) ) );
                }

                painter.setFont( *font );

                // need to manually add xpos until we have x-cliping.
                painter.drawText( last_x_pos,
                                 intersectedRect.y(),
                                 intersectedRect.width(),
                                 intersectedRect.height(),
                                  Qt::TextExpandTabs,
                                  subString.toString() );
                
                // Update the local variables
                last_x_pos += intersectedRect.width();
                intersectedRect.translate( intersectedRect.width(), 0 );
                currentColor = colors[i][chunkPos];
                currentAttrib = attribs[i][chunkPos];
                currentOnCursor = ON_CURSOR(chunkPos, i);
                chunkStart=chunkPos;
            }
        }
        // draw whatever remains.
        color = term_get_fg_color(currentAttrib, currentColor);
        painter.setPen( QColor( (color >> 16) & 0xFF,
                                (color >> 8 ) & 0xFF,
                                (color & 0xFF) ) );
        subString = qString.midRef( chunkStart, chunkPos - chunkStart);
        if (recalcSubString) {
            getRenderedStringRect( subString.toString(),
                                   currentAttrib,
                                   NULL,
                                   &intersectedRect );
        }
        painter.setFont( *font );
        painter.drawText( last_x_pos, intersectedRect.y(),
                          intersectedRect.width(), intersectedRect.height(),
                          Qt::TextExpandTabs,
                          subString.toString(),
                          &intersectedRect );
    }
}

void QTerm::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
        // FIXME These first four are workarounds for bugs in QT. Remove once it is fixed
        case Qt::Key_CapsLock:
        case Qt::Key_Shift:
            break;
        case Qt::Key_Return:
            term_send_data( terminal, "\n", 1 );
            break;
        case Qt::Key_Backspace:
            term_send_data( terminal, "\b", 1 );
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
        resize_term();
#else
        term_resize( terminal, event->size().width() / char_width, event->size().height() / char_height, 0 );
#endif
    }
    QWidget::update(0, 0,
                    size().width(), size().height());
}

#ifdef FAKE_MAIN
extern "C" int _main(int argc, char *argv[]);

int fake_main(term_t handle, int argc, char *argv[])
{
    return _main(argc, argv);
}
#endif

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

term_t init_term(int argc, char *argv[])
{
    term_t terminal;

    if( !term_create( &terminal ) ) {
        fprintf(stderr, "Failed to create terminal (%s)\n", strerror( errno ) );
        exit(1);
    }

    // This allows qterm to be used as a library. This is a workaround until
    // the infrastructure for having applications communicate is finished
#ifdef FAKE_MAIN
    if( !term_set_program( terminal, argv[0]) ) {
        fprintf(stderr, "Couldn't set argv[0] (%s)\n", strerror( errno ) );
        exit(1);
    }
    if( !term_set_fork_callback( terminal, fake_main ) ) {
        fprintf(stderr, "Couldn't set fake main (%s)\n", strerror( errno ) );
        exit(1);
    }
#endif

    //term_set_emulation( terminal, TERM_TYPE_ANSI );
    if( !term_begin( terminal, WIDTH, HEIGHT, 0 ) ) {
        fprintf(stderr, "Failed to begin terminal (%s)\n", strerror( errno ) );
        exit(1);
    }

	return terminal;
}

int init_ui(term_t terminal, int argc, char *argv[])
{
	QApplication app(argc, argv);
 
	QTerm term(NULL, terminal);
	term.show();

	return app.exec();
}

int main(int argc, char *argv[])
{
	term_t terminal;

#ifdef __QNX__
    logging_init();
#endif

	// The initialization is split in two steps to work around the multithreaded
	// fork bug in qnx
	terminal = init_term( argc, argv );
	return init_ui( terminal, argc, argv );
}
