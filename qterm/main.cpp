#include <QApplication>
#include <libterm.h>
#include <stdio.h>
#include <errno.h>
#include "qscrollterm.h"
#include "term_logging.h"

#define WIDTH    80
#define HEIGHT   100

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
	QApplication app(argc, argv);
 
	QScrollTerm term(NULL, terminal);
#ifdef __QNX__
    term.showFullScreen();
#endif
	term.show();

	return app.exec();
}

int main(int argc, char *argv[])
{
	term_t terminal;

#ifdef __QNX__
    logging_init();
#endif

	// The initialization is split in two steps to work around the multithreaded
	// fork bug in qnx
	terminal = init_term( argc, argv );
	return init_ui( terminal, argc, argv );
}
