#ifndef __TERMINALSCREEN_HPP__
#define __TERMINALSCREEN_HPP__

#include <bb/cascades/AbstractPane>
#include <bb/cascades/ListView>
#include <bb/cascades/QListDataModel>
#include <libterm.h>

class TerminalScreen : public QObject
{
    Q_OBJECT
public:
	TerminalScreen( term_t handle, int argc, char *argv[], bb::cascades::AbstractPane *);
	~TerminalScreen();
private:
    static void term_update( term_t term, int x, int y, int width, int height );

    int width;
    int height;
    term_t terminal;
    QSocketNotifier *notifier;
	bb::cascades::ListView *mStringList;
	bb::cascades::QStringListDataModel *mStringListModel;
private slots:
    void terminal_data();
};

#endif
