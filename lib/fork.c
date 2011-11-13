#ifdef __QNX__
#include <unix.h>
#else
#include <pty.h>
#endif
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <libterm_internal.h>

bool term_fork(term_t_i *term)
{
	bool inspace;
	int count = 1;
	char *ptr;
	char **args;
	char *shell;
	int pid;
	struct passwd *passwd;
	pid = forkpty(&term->fd, NULL, NULL, NULL);
	
	if(pid < 0 ) {
		return false;
	} else if( pid == 0 ) {
		// Default shell if no other is specified
		shell = "/bin/sh -l";
		// See if the application is overriding the shell
		if( term->shell != NULL ) {
			shell = term->shell;
		} else {
			// See if the environment has a shell set
			passwd = getpwuid( getuid( ) );
			if( passwd != NULL && passwd->pw_shell != NULL ) {
				shell = passwd->pw_shell;
			}
		}

		// Break up the command line where there are spaces
		// Count the tokens
		for( inspace = true, ptr = shell; *ptr != '\0'; ptr ++ ) {
			if( *ptr != ' ' && inspace ) {
				count ++;
			}
			inspace = (*ptr == ' ');
		}

		// Allocate space for tokens
		args = malloc( sizeof( char * ) * count );

		// Package the tokens
		count = 0;
		for( inspace = true, ptr = shell; *ptr != '\0'; ptr ++ ) {
			if( *ptr != ' ' && inspace ) {
				args[ count ] = ptr;
				count ++;
			} else if ( *ptr == ' ' && !inspace ) {
				*ptr = '\0';
				inspace = true;
				continue;
			}
			inspace = (*ptr == ' ');
		}

		// And a terminator
		args[ count ] = NULL;
		execvp( args[ 0 ], args );
	} else {
		term->child = pid;
	}
	return true;
}

void term_slay(term_t_i *term)
{
	kill(term->child, SIGTERM);
}
