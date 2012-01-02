#if defined(__QNX__)
#include <unix.h>
#elif defined(__MACH__)
#include <util.h>
#else
#include <pty.h>
#endif
#include <pwd.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <libterm_internal.h>

bool term_fork(term_t_i *term)
{
    bool inspace;
    int count = 2;
    char *ptr;
    char **args;
    char *shell;
    int status;
    int pid;
    int read_byte;
    int pipefd[2];
    struct passwd *passwd;
    struct winsize ws;

    if( pipe( pipefd ) ) {
        return false;
    }

    ws.ws_row = term->grid.height;
    ws.ws_col = term->grid.width;
    ws.ws_xpixel = term->grid.width;
    ws.ws_ypixel = term->grid.height;
    pid = forkpty(&term->fd, NULL, NULL, &ws);

    if(pid < 0 ) {
        return false;
    } else if( pid == 0 ) {
        // Default shell if no other is specified
        shell = "/bin/sh";
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

        if( term->fork == NULL ) {
            switch( term->type ) {
                case TERM_TYPE_VT100:
                    setenv("TERM", "vt100", 1);
                    break;
                case TERM_TYPE_ANSI:
                    setenv("TERM", "ansi", 1);
                    break;
                case TERM_TYPE_XTERM_COLOR:
                    setenv("TERM", "xterm-color", 1);
                    break;
                default:
                    // Unknown terminal type, default to VT100
                    setenv("TERM", "vt100", 1);
                    break;
            }
        }

        close( pipefd[ 0 ] );
        fcntl( pipefd[ 1 ], F_SETFD, FD_CLOEXEC );

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
        if( args == NULL ) {
            exit(1);
        }

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
        if( term->isShell ) {
            args[ count++ ] = "-l";
        }
        args[ count ] = NULL;
        if( term->fork == NULL ) {
            execvp( args[ 0 ], args );
            status = errno;
            write( pipefd[ 1 ], &status, 1 );
        } else {
            close( pipefd[ 1 ] );
            status = term->fork( TO_H(term), count, args );
        }
        exit( status );
    } else {
        close( pipefd[ 1 ] );
        term->child = pid;
        while( 1 ) {
            read_byte = read( pipefd[ 0 ], &status, 1 );
            if( read_byte == 1 ) {
                waitpid( term->child, &status, 0 );
                close( pipefd[ 0 ] );
                errno = WEXITSTATUS( status );
                return false;
            } else {
                if( errno == EINTR ) {
                    continue;
                } else {
                    break;
                }
            }
        }
    }
    return true;
}

void term_slay(term_t_i *term)
{
    if( term->child != 0 ) {
        kill(term->child, SIGTERM);
    }
}
