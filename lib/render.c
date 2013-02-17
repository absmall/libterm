#include <stdlib.h>
#include <string.h>
#include <libterm_internal.h>
#include "term_logging.h"

void term_process_output_data(term_t_i *term, char *buf, int length)
{
    int i, consumed;

    for( i = 0; i < length; i ++ ) {
#if 0
        if( isgraph( buf[ i ] ) ) {
            printf( "Output character %d (%c)\n", buf[ i ], buf[ i ] );
        } else {
            printf( "Output character %d\n", buf[ i ] );
        }
#endif
    }

    // Copy the characters for later processing.
    // TODO: Try to reduce the copying to only do this if necessary - like run directly
    // on this data if there's nothing pending, and make this into a ringbuffer
    if( term->output_max_bytes < term->output_byte_count + length ) {
        term->output_max_bytes = term->output_byte_count + length;
        if( term->output_bytes != NULL ) {
            term->output_bytes = realloc( term->output_bytes, term->output_max_bytes );
        } else {
            term->output_bytes = malloc( term->output_max_bytes );
        }
    }
    memcpy( term->output_bytes + term->output_byte_count, buf, length );
    term->output_byte_count += length;

    // Process all the characters 
    while( term->output_byte_count ) {
        // Process any escape codes. If one is still pending, abort processing, we'll
        // continue when there is more data
        consumed = term_send_escape( term, term->output_bytes, term->output_byte_count );
        if( consumed == -1 ) {
            // Not an escape code
        } else if( consumed == 0 ) {
            // It's a prefix, wait for more information
            break;
        } else {
            // It's an escape code, eat it and continue
            memmove( term->output_bytes, term->output_bytes + consumed, term->output_byte_count - consumed );
            term->output_byte_count -= consumed;
            continue;
        }

        // If it's not an escape, it's a regular character
        if( term->crow >= 0 && term->crow < term->grid.history && term->ccol >= 0 && (term->ccol < term->grid.width || term->autoexpand) ) {
            if( term->ccol > term->grid.width + term->extrawidth ) {
                // Autoexpanding
                term->extrawidth = term->ccol;
                term_resize_internal( TO_H( term ), term->grid.width, term->grid.height, term->grid.history - term->grid.height, term->extrawidth, NULL );
            }
            term->grid.grid[term->crow][term->ccol] = term->output_bytes[0];
            term->grid.attribs[term->crow][term->ccol] = term->cattr;
            term->grid.colours[term->crow][term->ccol] = term->ccolour;
            term_add_dirty_rect( term, term->ccol, term->crow, 1, 1 );
        } else {
            slog("Skipped writing %c out of bounds at (%d,%d)", term->output_bytes[0], term->crow, term->ccol);
        }
        memmove( term->output_bytes, term->output_bytes + 1, term->output_byte_count - 1 );
        term->output_byte_count --;
        term->ccol++;
        term->dirty_cursor.exists = true;
    }

    term_update( term );
    term_cursor_update( term );
}
