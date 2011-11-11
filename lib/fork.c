#include <pty.h>
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
		execl("/bin/sh", "/bin/sh", "-l", NULL);
	} else {
		term->child = pid;
	}
	return true;
}

void term_slay(term_t_i *term)
{
	kill(term->child, SIGTERM);
}
