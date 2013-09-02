#include <bps/virtualkeyboard.h>
#include <qvkbhandler.h>
#include <unistd.h>
#include "term_logging.h"

QAbstractEventDispatcher::EventFilter QVkbHandler::prevFilter;
QVkbHandler *QVkbHandler::instance = NULL;

QVkbHandler::QVkbHandler()
{
    if( instance == NULL ) {
        instance = this;
    } else {
        throw "Singleton error";
    }

    keyboardVisible = false;
    virtualkeyboard_request_events(0);
    virtualkeyboard_show();
    prevFilter = QAbstractEventDispatcher::instance()->setEventFilter(eventFilter);
}

QVkbHandler::~QVkbHandler()
{
    instance = NULL;
}

void QVkbHandler::resize()
{
    QWidget *child;
    int kbd_height = 0;
    if( keyboardVisible ) {
        virtualkeyboard_get_height( &kbd_height );
    }
    slog("Do resize");
    child = ((QWidget *)children()[0]);
    child->setGeometry(0, 0, width(), height()-kbd_height);
    slog("Resize done");
}

void QVkbHandler::resizeEvent(QResizeEvent *event)
{
    resize();
}

bool QVkbHandler::eventFilter(void *message)
{
    bps_event_t * const event = static_cast<bps_event_t *>(message);
 
    if (event && bps_event_get_domain(event) == virtualkeyboard_get_domain()) {
        const int id = bps_event_get_code(event);
        switch( id ) {
            case VIRTUALKEYBOARD_EVENT_VISIBLE:
                slog("Keyboard visible");
                instance->keyboardVisible = true;
                instance->resize();
                break;
            case VIRTUALKEYBOARD_EVENT_HIDDEN:
                slog("Keyboard hidden");
                instance->keyboardVisible = false;
                instance->resize();
                break;
            case VIRTUALKEYBOARD_EVENT_INFO:
                slog("Keyboard event");
                instance->resize();
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
