#include <libterm_internal.h>

#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

void term_add_dirty_rect(term_t_i *term, int x, int y, int width, int height)
{
    if( term->dirty.exists ) {
        int old_x2, old_y2, new_x2, new_y2;

        old_x2 = term->dirty.x + term->dirty.width;
        old_y2 = term->dirty.y + term->dirty.height;
        new_x2 = x + width;
        new_y2 = y + height;

        term->dirty.x = MIN( term->dirty.x, x );
        term->dirty.y = MIN( term->dirty.x, y );
        term->dirty.width = MAX( old_x2, new_x2 ) - term->dirty.x;
        term->dirty.height = MAX( old_y2, new_y2 ) - term->dirty.y;
    } else {
        term->dirty.x = x;
        term->dirty.y = y;
        term->dirty.width = width;
        term->dirty.height = height;
        term->dirty.exists = true;
    }
}
