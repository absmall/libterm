#include <stdlib.h>
#include "libterm_internal.h"

bool term_create(int width, int height, int scrollback, term_t *t)
{
	int i;
	term_t_i *term = malloc(sizeof(term_t_i));

	if( term == NULL ) {
		return false;
	}

	term->width = width;
	term->height = height;
	term->history = height + scrollback;

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
	}

	*t = TO_H(term);
	return true;
}

void term_free(term_t handle)
{
	int i;
	term_t_i *term = TO_S(handle);

	for( i = 0; i < term->history; i ++ ) {
		free(term->grid[i]);
		free(term->attribs[i]);
	}
	free(term->grid);
	free(term->attribs);
	free(term);
}
