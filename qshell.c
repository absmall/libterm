#include <stdio.h>
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
	fd_set rfds;
	int file_handle;
	term_t handle;
	term_create(WIDTH, HEIGHT, 0, &handle);

	term_register_update(handle, update);
	file_handle = term_get_file_descriptor(handle);

	FD_ZERO(&rfds);
	FD_SET(file_handle, &rfds);

	select( file_handle+1, &rfds, NULL, NULL, NULL );
	term_process_child(handle);

	term_free(handle);

	return 0;
}
