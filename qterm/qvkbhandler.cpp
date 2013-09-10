#include <bps/virtualkeyboard.h>
#include <bps/navigator.h>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
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
    navigator_request_events(0);
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
 
    if (event ) {
        if (bps_event_get_domain(event) == virtualkeyboard_get_domain()) {
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
        } else if( bps_event_get_domain(event) == navigator_get_domain()) {
            const int id = bps_event_get_code(event);
            switch( id ) {
                case NAVIGATOR_SWIPE_DOWN:
                    {
                        virtualkeyboard_hide();
                        QDialog dialog;
                        QPushButton button;
                        QTextEdit textEdit;
                        button.setText("Ok");
                        QVBoxLayout extensionLayout;
                        extensionLayout.setMargin(0);
                        extensionLayout.addWidget(&textEdit);
                        extensionLayout.addWidget(&button);
                        dialog.setLayout(&extensionLayout);
                        QObject::connect(&button, SIGNAL(pressed()), &dialog, SLOT(accept()));
                        textEdit.setText("<h2>Help</h2>\n\n"
                                        "<h3>Special characters</h3>\n\n"
                                        "To type special characters, touching anywhere on the display will cause a wheel to appear. The wheel enables typing the arrow keys, tab, escape, tilde and pipe characters. To select characters from the wheel, while continuing to touch, slide your finger into the region of the character you wish to type, then back to the center of the wheel. You may do this multiple times to type multiple characters without removing your finger from the screen.\n\n"
                                        "<h3>Scrolling</h3>\n\n"
                                        "To see history, hold two fingers on the display and slide both fingers up or down.");
                        textEdit.setReadOnly(true);
                        dialog.exec();
                        virtualkeyboard_show();
                    }
                    break;
            }
        }
    }
 
    if (prevFilter)
        return prevFilter(message);
    else
        return false;
}
