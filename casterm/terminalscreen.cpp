#include "terminalscreen.hpp"
#include "logging.h"

using namespace bb::cascades;

TerminalScreen::TerminalScreen( term_t handle, int argc, char *argv[], AbstractPane *pane) : terminal(handle)
{
    int ret = 0;
    QObject *pObj;
    QVariantMap map;

    mRecordingDeviceListModel = new QVariantListDataModel;
    //term_set_user_data( terminal, this );
    //term_register_update( terminal, term_update );
    pObj = pane->findChild<QObject *>("terminalLines");
    mRecordingDeviceList = (ListView *) pObj;
    for( int i = 0; i < 3; i ++ ) {
        map["line"] = QString("Hello");
        *mRecordingDeviceListModel << map;
    }
    mRecordingDeviceList->setDataModel( mRecordingDeviceListModel );
}

TerminalScreen::~TerminalScreen()
{
}

void TerminalScreen::term_update( term_t term, int x, int y, int width, int height )
{
    TerminalScreen *ts = (TerminalScreen *)term_get_user_data( term );
    slog2fa(buffer_handle, 0, SLOG2_INFO, "terminal update");
}
