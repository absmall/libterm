#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <libterm_internal.h>

bool term_allocate_grid(term_grid *grid)
{
    int i, j;

    grid->grid = malloc(sizeof(uint32_t *)*grid->history);
    if( grid->grid == NULL ) {
        errno = ENOMEM;
        return false;
    }
    grid->attribs = malloc(sizeof(uint32_t *)*grid->history);
    if( grid->attribs == NULL ) {
        free(grid->grid);
        errno = ENOMEM;
        return false;
    }
    grid->colours = malloc(sizeof(uint32_t *)*grid->history);
    if( grid->colours == NULL ) {
        free(grid->attribs);
        free(grid->grid);
        errno = ENOMEM;
        return false;
    }
    for( i = 0; i < grid->history; i ++ ) {
        grid->grid[i] = malloc(sizeof(uint32_t)*grid->width);
        if( grid->grid[i] == NULL ) {
            for(i--; i>=0; i--) {
                free(grid->grid[i]);
            }
            free(grid->grid[0]);
            free(grid->colours);
            free(grid->attribs);
            free(grid->grid);
            errno = ENOMEM;
            return false;
        }
        grid->attribs[i] = malloc(sizeof(uint32_t)*grid->width);
        if( grid->attribs[i] == NULL ) {
            free(grid->grid[i]);
            for(i--; i>=0; i--) {
                free(grid->attribs[i]);
                free(grid->grid[i]);
            }
            free(grid->attribs[0]);
            free(grid->grid[0]);
            free(grid->colours);
            free(grid->attribs);
            free(grid->grid);
            errno = ENOMEM;
            return false;
        }
        grid->colours[i] = malloc(sizeof(uint32_t)*grid->width);
        if( grid->colours[i] == NULL ) {
            free(grid->attribs[i]);
            free(grid->grid[i]);
            for(i--; i>=0; i--) {
                free(grid->colours[i]);
                free(grid->attribs[i]);
                free(grid->grid[i]);
            }
            free(grid->grid[0]);
            free(grid->attribs[0]);
            free(grid->colours);
            free(grid->attribs);
            free(grid->grid);
            errno = ENOMEM;
            return false;
        }
        for( j = 0; j < grid->width; j ++ ) {
            grid->grid[ i ][ j ] = ' ';
        }
        memset(grid->attribs[i], 0, sizeof(uint32_t)*grid->width);
        memset(grid->colours[i], 0, sizeof(uint32_t)*grid->width);
    }

    return true;
}

void term_shiftrows(term_t_i *term)
{
    uint32_t *firstrow;
    int i;

    // Just cycle the pointers, and move the first row to the end, but clear it
    // grid
    firstrow = term->grid.grid[0];
    for( i = 1; i < term->grid.history; i ++ ) {
        term->grid.grid[ i - 1 ] = term->grid.grid[ i ];
    }
    term->grid.grid[ i - 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = ' ';
    }

    // attribs
    firstrow = term->grid.attribs[0];
    for( i = 1; i < term->grid.history; i ++ ) {
        term->grid.attribs[ i - 1 ] = term->grid.attribs[ i ];
    }
    term->grid.attribs[ i - 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }

    // colours
    firstrow = term->grid.colours[0];
    for( i = 1; i < term->grid.history; i ++ ) {
        term->grid.colours[ i - 1 ] = term->grid.colours[ i ];
    }
    term->grid.colours[ i - 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }

    term->crow --;
}

void term_release_grid(term_grid *grid)
{
    int i;

    for( i = 0; i < grid->history; i ++ ) {
        free(grid->grid[i]);
        free(grid->attribs[i]);
        free(grid->colours[i]);
    }
    free(grid->colours);
    free(grid->attribs);
    free(grid->grid);
}

