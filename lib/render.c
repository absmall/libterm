#include <stdlib.h>
#include <libterm_internal.h>

void term_process_output_data(term_t_i *term, char *buf, int length)
{
    int i;

    for( i = 0; i < length; i ++ ) {
#if 0
        if( isgraph( buf[ i ] ) ) {
            printf( "Output character %d (%c)\n", buf[ i ], buf[ i ] );
        } else {
            printf( "Output character %d\n", buf[ i ] );
        }
#endif

        if( term->escape_mode ) {
            i += term_send_escape( term, buf + i, length  - i );
            if( i == length ) break;
        }
        switch(buf[i]) {
            case 7:
                // Bell
                if( term->bell != NULL ) term->bell(TO_H(term));
                break;
            case '\b':
                term->ccol--;
                term->dirty_cursor.exists = true;
                break;
            case '\r':
                term->ccol=0;
                break;
            case '\n':
                term->ccol=0;
                term->crow++;
                term->dirty_cursor.exists = true;
                if( term->crow >= term->grid.history ) {
                    term_shiftrows(term);
                    term_add_dirty_rect( term, 0, 0, term->grid.width, term->grid.height );
                }
                break;
            case 27:
                term->escape_mode = true;
                term_send_escape( term, buf + i, 1 );
                break;
            default:
                if( term->crow < term->grid.history && term->ccol < term->grid.width ) {
                    term->grid.grid[term->crow][term->ccol] = buf[i];
                    term->grid.attribs[term->crow][term->ccol] = term->cattr;
                    term->grid.colours[term->crow][term->ccol] = term->ccolour;
                    term_add_dirty_rect( term, term->crow, term->ccol, 1, 1 );
                }
                term->ccol++;
                term->dirty_cursor.exists = true;
        }
    }

    term_update( term );
    term_cursor_update( term );
}
