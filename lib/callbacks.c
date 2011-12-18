#include <libterm_internal.h>

void term_update(term_t_i *term)
{
    if( term->dirty.exists && term->update != NULL ) {
        term->update(TO_H(term), 0, 0, term->grid.width, term->grid.height);
        term->dirty.exists = false;
    }
}

void term_cursor_update(term_t_i *term)
{
    if( term->dirty_cursor.exists && term->cursor_update != NULL ) {
        term->cursor_update(TO_H(term), term->dirty_cursor.old_ccol, term->dirty_cursor.old_crow - term->row, term->ccol, term->crow - term->row);
        term->dirty_cursor.exists = false;
        term->dirty_cursor.old_ccol = term->ccol;
        term->dirty_cursor.old_crow = term->crow;
    }
}
