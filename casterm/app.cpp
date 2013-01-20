#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

#include "app.hpp"

using namespace bb::cascades;

App::App(term_t handle, int argc, char **argv) : Application(argc, argv)
{
    QmlDocument *qml = QmlDocument::create("app/native/assets/main.qml");
    qml->setContextProperty("cs", this);

    AbstractPane *root = qml->createRootObject<AbstractPane>();
    setScene(root);


    mTerminalScreen = new TerminalScreen(handle, argc, argv, root);
}

App::~App()
{
	delete mTerminalScreen;
}
