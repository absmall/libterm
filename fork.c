#include <pty.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <libterm_internal.h>

bool term_fork(term_t_i *term)
{
	int pid;
	pid = forkpty(&term->fd, NULL, NULL, NULL);
	
	if(pid < 0 ) {
		printf("Fork failed\n");
		return false;
	} else if( pid == 0 ) {
		execl("/bin/sh", "/bin/sh", "-l", NULL);
		printf("After exec\n");
	} else {
		term->child = pid;
	}
}

void term_slay(term_t_i *term)
{
	kill(term->child, SIGTERM);
}
