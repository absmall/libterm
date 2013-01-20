#include <bb/cascades/Application>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>

#include "app.hpp"
#include "logging.h"

using ::bb::cascades::Application;

int main(int argc, char **argv)
{
    App app(argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name(); 
    QString filename = QString( "audioDiag_%1" ).arg( locale_string );
    logging_init();
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    return app.exec();
}

