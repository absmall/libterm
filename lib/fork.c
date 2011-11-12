#ifdef __QNX__
#include <unix.h>
#else
#include <pty.h>
#endif
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <libterm_internal.h>

bool term_fork(term_t_i *term)
{
	int pid;
	pid = forkpty(&term->fd, NULL, NULL, NULL);
	
	if(pid < 0 ) {
		return false;
	} else if( pid == 0 ) {
		if( term->shell != NULL ) {
			bool inspace;
			int count = 1;
			char *ptr;
			char **args;

			// Count the tokens
			for( inspace = true, ptr = term->shell; *ptr != '\0'; ptr ++ ) {
				if( *ptr != ' ' && inspace ) {
					count ++;
				}
				inspace = (*ptr == ' ');
			}

			// Allocate space for tokens
			args = malloc( sizeof( char * ) * count );

			// Package the tokens
			count = 0;
			for( inspace = true, ptr = term->shell; *ptr != '\0'; ptr ++ ) {
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
			execl("/bin/sh", "/bin/sh", "-l", NULL);
		}
	} else {
		term->child = pid;
	}
	return true;
}

void term_slay(term_t_i *term)
{
	kill(term->child, SIGTERM);
}
