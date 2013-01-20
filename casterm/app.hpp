#ifndef __APP_H__
#define __APP_H__

#include <bb/cascades/Application>
#include "terminalscreen.hpp"

class App : public ::bb::cascades::Application 
{
  Q_OBJECT
public:
  App(int argc, char **argv);
    ~App();

private:
    TerminalScreen *mTerminalScreen;
};

#endif
