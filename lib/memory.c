#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <libterm_internal.h>

#define MIN(x,y) ((x)<(y)?(x):(y))

bool term_allocate_grid(term_grid *grid)
{
    int i, j;

    grid->grid = malloc(sizeof(wchar_t *)*(grid->history + grid->height));
    if( grid->grid == NULL ) {
        errno = ENOMEM;
        return false;
    }
    grid->attribs = malloc(sizeof(uint32_t *)*(grid->history + grid->height));
    if( grid->attribs == NULL ) {
        free(grid->grid);
        errno = ENOMEM;
        return false;
    }
    grid->colours = malloc(sizeof(uint32_t *)*(grid->history + grid->height));
    if( grid->colours == NULL ) {
        free(grid->attribs);
        free(grid->grid);
        errno = ENOMEM;
        return false;
    }
    for( i = 0; i < grid->history + grid->height; i ++ ) {
        grid->grid[i] = malloc(sizeof(uint32_t)*(grid->width+1));
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
        grid->grid[ i ][ j ] = 0;
        memset(grid->attribs[i], 0, sizeof(uint32_t)*grid->width);
        memset(grid->colours[i], 0, sizeof(uint32_t)*grid->width);
    }
    grid->grid += grid->history;
    grid->attribs += grid->history;
    grid->colours += grid->history;

    return true;
}

void term_copy_grid(term_grid *dst, term_grid *src)
{
    int i, j;
    int width, height;

    // Copy history
    width = MIN(dst->width, src->width);
    height = MIN(dst->history, src->history);
    for( i = 0; i < height; i ++ ) {
        for( j = 0; j < width; j ++ ) {
            dst->grid[-i][j] = src->grid[-i][j];
            dst->attribs[-i][j] = src->attribs[-i][j];
            dst->colours[-i][j] = src->colours[-i][j];
        }
    }
    // Copy grid
    height = MIN(dst->height, src->height);
    for( i = 0; i < height; i ++ ) {
        for( j = 0; j < width; j ++ ) {
            dst->grid[i][j] = src->grid[i][j];
            dst->attribs[i][j] = src->attribs[i][j];
            dst->colours[i][j] = src->colours[i][j];
        }
    }
}

void term_shiftrows_up(term_t_i *term)
{
    wchar_t *gridrow;
    uint32_t *firstrow;
    int i;

    // Just cycle the pointers, and move the first row to the end, but clear its
    // grid
    gridrow = term->grid.grid[-term->grid.history];
    for( i = -term->grid.history+1; i < term->grid.height; i ++ ) {
        term->grid.grid[ i - 1 ] = term->grid.grid[ i ];
    }
    term->grid.grid[ i - 1 ] = gridrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        gridrow[ i ] = ' ';
    }

    // attribs
    firstrow = term->grid.attribs[-term->grid.history];
    for( i = -term->grid.history+1; i < term->grid.height; i ++ ) {
        term->grid.attribs[ i - 1 ] = term->grid.attribs[ i ];
    }
    term->grid.attribs[ i - 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }

    // colours
    firstrow = term->grid.colours[-term->grid.history];
    for( i = -term->grid.history+1; i < term->grid.height; i ++ ) {
        term->grid.colours[ i - 1 ] = term->grid.colours[ i ];
    }
    term->grid.colours[ i - 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }

    term->crow --;
}

void term_shiftrows_down(term_t_i *term)
{
    wchar_t *gridrow;
    uint32_t *firstrow;
    int i;

    // Just cycle the pointers, and move the first row to the end, but clear its
    // grid
    gridrow = term->grid.grid[term->grid.height-1];
    for( i = term->grid.height-1; i >= -term->grid.history; i -- ) {
        term->grid.grid[ i ] = term->grid.grid[ i - 1 ];
    }
    term->grid.grid[ i + 1 ] = gridrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        gridrow[ i ] = ' ';
    }

    // attribs
    firstrow = term->grid.attribs[term->grid.height-1];
    for( i = term->grid.height-1; i >= -term->grid.history; i -- ) {
        term->grid.attribs[ i ] = term->grid.attribs[ i - 1 ];
    }
    term->grid.attribs[ i + 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }

    // colours
    firstrow = term->grid.colours[term->grid.height-1];
    for( i = term->grid.height-1; i >= -term->grid.history; i -- ) {
        term->grid.colours[ i ] = term->grid.colours[ i - 1 ];
    }
    term->grid.colours[ i + 1 ] = firstrow;

    for( i = 0; i < term->grid.width; i ++ ) {
        firstrow[ i ] = 0;
    }
}

void term_release_grid(term_grid *grid)
{
    int i;

    grid->grid -= grid->history;
    grid->attribs -= grid->history;
    grid->colours -= grid->history;
    for( i = 0; i < grid->history + grid->height; i ++ ) {
        free(grid->grid[i]);
        free(grid->attribs[i]);
        free(grid->colours[i]);
    }
    free(grid->colours);
    free(grid->attribs);
    free(grid->grid);
}

