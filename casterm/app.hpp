#ifndef __APP_H__
#define __APP_H__

#include <bb/cascades/Application>
#include "terminalscreen.hpp"
#include <libterm.h>

class App : public ::bb::cascades::Application 
{
  Q_OBJECT
public:
  App(term_t handle, int argc, char **argv);
    ~App();

private:
    TerminalScreen *mTerminalScreen;
};

#endif
