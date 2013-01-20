#include "terminalscreen.hpp"
#include "logging.h"

using namespace bb::cascades;

TerminalScreen::TerminalScreen( term_t handle, int argc, char *argv[], AbstractPane *pane) : terminal(handle)
{
    int ret = 0;
    QObject *pObj;

    term_get_grid_size( handle, &width, &height );
    slog2f(buffer_handle, 0, SLOG2_INFO, "TerminalScreen start %dx%d", width, height);
    mStringListModel = new QStringListDataModel;
    term_set_user_data( terminal, this );
    term_register_update( terminal, term_update );
    pObj = pane->findChild<QObject *>("terminalLines");
    mStringList = (ListView *) pObj;
    mStringList->setDataModel( mStringListModel );

    for( int i = 0; i < height; i ++ ) {
        *mStringListModel << QString(term_get_line(terminal, i));
    }

    notifier = new QSocketNotifier( term_get_file_descriptor(terminal), QSocketNotifier::Read );
    QObject::connect(notifier, SIGNAL(activated(int)), this, SLOT(terminal_data()));
}

TerminalScreen::~TerminalScreen()
{
    delete notifier;
}

void TerminalScreen::term_update( term_t term, int x, int y, int width, int height )
{
    TerminalScreen *ts = (TerminalScreen *)term_get_user_data( term );
    slog2c(buffer_handle, 0, SLOG2_INFO, "terminal update");
    for( int i = 0; i < height; i ++ ) {
        ts->mStringListModel->replace(i, QString(term_get_line(ts->terminal, i)));
    }
}

void TerminalScreen::terminal_data()
{
    if( !term_process_child( terminal ) ) {
        exit(0);
    }
}
