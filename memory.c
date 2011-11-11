#include <string.h>
#include <stdlib.h>
#include <libterm_internal.h>

bool term_allocate_grid(term_t_i *term)
{
	int i, j;

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
		for( j = 0; j < term->width; j ++ ) {
			term->grid[ i ][ j ] = ' ';
		}
		memset(term->attribs[i], 0, sizeof(uint32_t)*term->width);
	}

	return true;
}

void term_shiftrows(term_t_i *term)
{
	uint32_t *firstrow;
	int i;

	// Just cycle the pointers, and move the first row to the end, but clear it
	firstrow = term->grid[0];
	for( i = 1; i < term->history; i ++ ) {
		term->grid[ i - 1 ] = term->grid[ i ];
	}
	term->grid[ i - 1 ] = firstrow;
	for( i = 0; i < term->width; i ++ ) {
		firstrow[ i ] = ' ';
	}
	term->crow --;
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
