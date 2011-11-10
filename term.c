#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include "libterm_internal.h"

bool term_allocate_grid(term_t_i *term)
{
	int i;

	term->grid = malloc(sizeof(uint32_t *)*term->history);
	if( term->grid == NULL ) {
		free(term);
		return false;
	}
	term->attribs = malloc(sizeof(uint32_t *)*term->history);
	if( term->attribs == NULL ) {
		free(term->grid);
		free(term);
		return false;
	}
	for( i = 0; i < term->history; i ++ ) {
		term->grid[i] = malloc(sizeof(uint32_t)*term->width);
		if( term->grid[i] == NULL ) {
			for(i--; i>=0; i--) {
				free(term->grid[i]);
			}
			free(term->grid[0]);
			free(term->grid);
			free(term);
			return false;
		}
		term->attribs[i] = malloc(sizeof(uint32_t)*term->width);
		if( term->attribs[i] == NULL ) {
			free(term->grid[i]);
			for(i--; i>=0; i--) {
				free(term->grid[i]);
				free(term->attribs[i]);
			}
			free(term->grid[0]);
			free(term->attribs[0]);
			free(term->grid);
			free(term);
			return false;
		}
		memset(term->grid[i], 0, sizeof(uint32_t)*term->width);
		memset(term->attribs[i], 0, sizeof(uint32_t)*term->width);
	}

	return true;
}

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

void term_process_child(term_t handle)
{
	int length;
	char buf[100];
	term_t_i *term;

	term = TO_S(handle);

	length = read( term->fd, buf, 100 );
	term_process_output_data(term, buf, length);
}

void term_send_data(term_t handle, char *string, int length)
{
	term_t_i *term;

	term = TO_S(handle);

	write(term->fd, string, length);
}

void term_release_grid(term_t_i *term)
{
	int i;

	for( i = 0; i < term->history; i ++ ) {
		free(term->grid[i]);
		free(term->attribs[i]);
	}
	free(term->grid);
	free(term->attribs);
	free(term);
}

bool term_create(int width, int height, int scrollback, term_t *t)
{
	term_t_i *term = malloc(sizeof(term_t_i));

	if( term == NULL ) {
		return false;
	}

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

	*t = TO_H(term);
	return true;
}

void term_free(term_t handle)
{
	term_t_i *term = TO_S(handle);

	term_slay(term);
	term_release_grid(term);
}
