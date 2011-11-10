#include <stdio.h>
#include <unistd.h>
#include <libterm.h>
#include <sys/select.h>

#define WIDTH	80
#define HEIGHT	25

void update(term_t handle, int x, int y, int width, int height)
{
	int i, j;
	const uint32_t **grid = term_get_grid(handle);

	for( i = 0; i < HEIGHT; i ++ ) {
		for( j = 0; j < WIDTH; j ++ ) {
			fputc( grid[i][j], stdout );
		}
		printf( "\n" );
	}
}

int main(int argc, char *argv[])
{
	int length;
	fd_set rfds;
	int file_handle;
	int stdin_handle;
	term_t handle;
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
			term_process_child(handle);
		} else if(FD_ISSET(stdin_handle, &rfds)) {
			char buf[100];
			length = read(stdin_handle, buf, 100);
			term_send_data(handle, buf, length);
		}

	}

	term_free(handle);

	return 0;
}
