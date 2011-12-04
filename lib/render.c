#include <stdlib.h>
#include <libterm_internal.h>

void term_process_output_data(term_t_i *term, char *buf, int length)
{
    bool change = false;
    bool cursor_change = false;
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
            change = true;
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
                cursor_change = true;
                break;
            case '\r':
                break;
            case '\n':
                term->ccol=0;
                term->crow++;
                cursor_change = true;
                if( term->crow >= term->grid.history ) {
                    term_shiftrows(term);
                    change = true;
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
                    change = true;
                }
                term->ccol++;
                cursor_change = true;
        }
    }

    if( change && term->update != NULL ) term->update(TO_H(term), 0, 0, term->grid.width, term->grid.height);
    if( cursor_change && term->cursor_update != NULL ) term->cursor_update(TO_H(term), term->ccol, term->crow - term->row);
}
