#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <libterm.h>
#include <sys/select.h>

#define WIDTH	80
#define HEIGHT	25

WINDOW *wnd;

void update(term_t handle, int x, int y, int width, int height)
{
	int i, j;
	const uint32_t **grid = term_get_grid(handle);


	for( i = 0; i < HEIGHT; i ++ ) {
		for( j = 0; j < WIDTH; j ++ ) {
			wmove( wnd, i, j );
			wdelch( wnd );
			winsch( wnd, grid[i][j] );
		}
	}
	wrefresh(wnd);
}

int main(int argc, char *argv[])
{
	int length;
	fd_set rfds;
	int file_handle;
	int stdin_handle;
	term_t handle;

	initscr();

	wnd = newwin(HEIGHT, WIDTH, 0, 0);
	wclear( wnd );

	term_create(WIDTH, HEIGHT, 0, &handle);

	term_register_update(handle, update);
	stdin_handle = fileno(stdin);
	file_handle = term_get_file_descriptor(handle);

	while( 1 ) {
		FD_ZERO(&rfds);
		FD_SET(file_handle, &rfds);
		FD_SET(stdin_handle, &rfds);

		select( file_handle+1, &rfds, NULL, NULL, NULL );

		if(FD_ISSET(file_handle, &rfds)) {
			if( !term_process_child(handle) ) {
				break;
			}
		} else if(FD_ISSET(stdin_handle, &rfds)) {
			char buf[100];
			length = read(stdin_handle, buf, 100);
			term_send_data(handle, buf, length);
		}

	}

	term_free(handle);

	endwin();

	return 0;
}
