#include <bb/cascades/Application>
#include <QtCore/QLocale>
#include <QtCore/QTranslator>
#include <errno.h>

#include "app.hpp"
#include "logging.h"

#define WIDTH 20
#define HEIGHT 10

using ::bb::cascades::Application;

#ifdef FAKE_MAIN
extern "C" int _main(int argc, char *argv[]);

int fake_main(term_t handle, int argc, char *argv[])
{
    return _main(argc, argv);
}
#endif

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
    App app(terminal, argc, argv);

    QTranslator translator;
    QString locale_string = QLocale().name(); 
    QString filename = QString( "audioDiag_%1" ).arg( locale_string );
    if (translator.load(filename, "app/native/qm")) {
        app.installTranslator( &translator );
    }

    return app.exec();
}

int main(int argc, char *argv[])
{
	term_t terminal;

    logging_init();

	// The initialization is split in two steps to work around the multithreaded
	// fork bug in qnx
	terminal = init_term( argc, argv );
	return init_ui( terminal, argc, argv );
}


