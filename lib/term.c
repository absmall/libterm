#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "libterm_internal.h"

void term_register_update(term_t handle, void (*update)(term_t handle, int x, int y, int width, int height))
{
	term_t_i *term;

	term = TO_S(handle);

	term->update = update;
}

int term_get_file_descriptor(term_t handle)
{
	term_t_i *term;

	term = TO_S(handle);

	return term->fd;
}

const uint32_t **term_get_grid(term_t handle)
{
	term_t_i *term;

	term = TO_S(handle);

	return (const uint32_t **)term->grid;
}

bool term_process_child(term_t handle)
{
	int length;
	char buf[100];
	term_t_i *term;

	term = TO_S(handle);

	length = read( term->fd, buf, 100 );
	if( length == -1 ) {
		return false;
	}
	term_process_output_data(term, buf, length);
	return true;
}

void term_send_data(term_t handle, const char *string, int length)
{
	term_t_i *term;

	term = TO_S(handle);

	write(term->fd, string, length);
}

bool term_create(term_t *t)
{
	term_t_i *term = malloc(sizeof(term_t_i));

	if( term == NULL ) {
		return false;
	}

	memset( term, 0, sizeof(term_t_i) );

	*t = TO_H(term);
	return true;
}

bool term_set_shell(term_t handle, char *shell)
{
	term_t_i *term = TO_S( handle );

	term->shell = strdup( shell );

	return term->shell != NULL;
}

bool term_begin(term_t handle, int width, int height, int scrollback)
{
	term_t_i *term = TO_S(handle);

	term->width = width;
	term->height = height;
	term->history = height + scrollback;

	if( !term_allocate_grid(term) ) {
		return false;
	}

	if( !term_fork(term) ) {
		term_release_grid(term);
		return false;
	}

	return true;
}

void term_free(term_t handle)
{
	term_t_i *term = TO_S(handle);

	term_slay( term );
	term_release_grid( term );
	if( term->shell != NULL ) {
		free( term->shell );
	}
	free( term );
}

void term_set_user_data(term_t handle, void *user_data)
{
	term_t_i *term = TO_S(handle);

	term->user_data = user_data;
}

void *term_get_user_data(term_t handle)
{
	term_t_i *term = TO_S(handle);

	return term->user_data;
}
