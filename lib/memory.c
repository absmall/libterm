#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <libterm_internal.h>

bool term_allocate_grid(term_t_i *term)
{
    int i, j;

    term->grid = malloc(sizeof(uint32_t *)*term->history);
    if( term->grid == NULL ) {
        free(term);
        errno = ENOMEM;
        return false;
    }
    term->attribs = malloc(sizeof(uint32_t *)*term->history);
    if( term->attribs == NULL ) {
        free(term->grid);
        free(term);
        errno = ENOMEM;
        return false;
    }
    term->colours = malloc(sizeof(uint32_t *)*term->history);
    if( term->colours == NULL ) {
        free(term->attribs);
        free(term->grid);
        free(term);
        errno = ENOMEM;
        return false;
    }
    for( i = 0; i < term->history; i ++ ) {
        term->grid[i] = malloc(sizeof(uint32_t)*term->width);
        if( term->grid[i] == NULL ) {
            for(i--; i>=0; i--) {
                free(term->grid[i]);
            }
            free(term->grid[0]);
            free(term->colours);
            free(term->attribs);
            free(term->grid);
            free(term);
            errno = ENOMEM;
            return false;
        }
        term->attribs[i] = malloc(sizeof(uint32_t)*term->width);
        if( term->attribs[i] == NULL ) {
            free(term->grid[i]);
            for(i--; i>=0; i--) {
                free(term->attribs[i]);
                free(term->grid[i]);
            }
            free(term->attribs[0]);
            free(term->grid[0]);
            free(term->colours);
            free(term->attribs);
            free(term->grid);
            free(term);
            errno = ENOMEM;
            return false;
        }
        term->colours[i] = malloc(sizeof(uint32_t)*term->width);
        if( term->colours[i] == NULL ) {
            free(term->attribs[i]);
            free(term->grid[i]);
            for(i--; i>=0; i--) {
                free(term->colours[i]);
                free(term->attribs[i]);
                free(term->grid[i]);
            }
            free(term->grid[0]);
            free(term->attribs[0]);
            free(term->colours);
            free(term->attribs);
            free(term->grid);
            free(term);
            errno = ENOMEM;
            return false;
        }
        for( j = 0; j < term->width; j ++ ) {
            term->grid[ i ][ j ] = ' ';
        }
        memset(term->attribs[i], 0, sizeof(uint32_t)*term->width);
        memset(term->colours[i], 0, sizeof(uint32_t)*term->width);
    }

    term->allocated = true;

    return true;
}

void term_shiftrows(term_t_i *term)
{
    uint32_t *firstrow;
    int i;

    // Just cycle the pointers, and move the first row to the end, but clear it
    // grid
    firstrow = term->grid[0];
    for( i = 1; i < term->history; i ++ ) {
        term->grid[ i - 1 ] = term->grid[ i ];
    }
    term->grid[ i - 1 ] = firstrow;

    for( i = 0; i < term->width; i ++ ) {
        firstrow[ i ] = ' ';
    }

    // attribs
    firstrow = term->attribs[0];
    for( i = 1; i < term->history; i ++ ) {
        term->attribs[ i - 1 ] = term->attribs[ i ];
    }
    term->attribs[ i - 1 ] = firstrow;

    for( i = 0; i < term->width; i ++ ) {
        firstrow[ i ] = 0;
    }

    // colours
    firstrow = term->colours[0];
    for( i = 1; i < term->history; i ++ ) {
        term->colours[ i - 1 ] = term->colours[ i ];
    }
    term->colours[ i - 1 ] = firstrow;

    for( i = 0; i < term->width; i ++ ) {
        firstrow[ i ] = 0;
    }

    term->crow --;
}

void term_release_grid(term_t_i *term)
{
    int i;

    if( !term->allocated ) return;
    for( i = 0; i < term->history; i ++ ) {
        free(term->grid[i]);
        free(term->attribs[i]);
        free(term->colours[i]);
    }
    free(term->colours);
    free(term->attribs);
    free(term->grid);

    term->allocated = false;
}

