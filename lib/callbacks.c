#include <libterm_internal.h>

void term_update(term_t_i *term)
{
    if( term->dirty.exists && term->update != NULL ) {
        term->update(TO_H(term), term->dirty.x, term->dirty.y, term->dirty.width, term->dirty.height);
        term->dirty.exists = false;
    }
}

void term_cursor_update(term_t_i *term)
{
    if( term->dirty_cursor.exists && term->cursor_update != NULL ) {
        term->cursor_update(TO_H(term), term->dirty_cursor.old_ccol, term->dirty_cursor.old_crow - (term->grid.history - term->grid.height), term->ccol, term->crow - (term->grid.history - term->grid.height));
        term->dirty_cursor.exists = false;
        term->dirty_cursor.old_ccol = term->ccol;
        term->dirty_cursor.old_crow = term->crow;
    }
}
