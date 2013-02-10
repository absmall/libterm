#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "libterm_internal.h"
#include "terminfo_commands.h"
#include "term_logging.h"

void term_register_update(term_t handle, void (*update)(term_t handle, int x, int y, int width, int height))
{
    term_t_i *term;

    term = TO_S(handle);

    term->update = update;
}

void term_register_cursor(term_t handle, void (*update)(term_t handle, int old_x, int old_y, int new_x, int new_y))
{
    term_t_i *term;

    term = TO_S(handle);

    term->cursor_update = update;
}

void term_register_bell(term_t handle, void (*ding)(term_t handle))
{
    term_t_i *term;

    term = TO_S(handle);

    term->bell = ding;
}

int term_get_file_descriptor(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return term->fd;
}

const wchar_t **term_get_grid(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const wchar_t **)term->grid.grid + term->grid.history - term->grid.height;
}

const uint32_t **term_get_attribs(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->grid.attribs + term->grid.history - term->grid.height;
}

const uint32_t **term_get_colours(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->grid.colours + term->grid.history - term->grid.height;
}

const char *term_get_line(term_t handle, int row)
{
    unsigned int length;
    term_t_i *term;
    wchar_t *grid_row;

    term = TO_S(handle);

    row += term->grid.history - term->grid.height;
    if( row < 0 || row >= term->grid.history ) {
        errno = EINVAL;
        return NULL;
    }

    grid_row = term->grid.grid[row];
    length = wcstombs(NULL, grid_row, 0) + 1;
    if( length > term->conversion_buffer_size ) {
        if( term->conversion_buffer != NULL ) {
            free( term->conversion_buffer );
        }
        term->conversion_buffer = malloc( length );
        if( term->conversion_buffer == NULL ) {
            errno = ENOMEM;
            term->conversion_buffer_size = 0;
            return NULL;
        }
        term->conversion_buffer_size = length;
    }
    wcstombs(term->conversion_buffer, grid_row, term->conversion_buffer_size);

    return term->conversion_buffer;
}

static int term_get_color(uint32_t attrib, uint32_t colour, uint32_t *color)
{
    if( attrib & TERM_ATTRIB_BOLD ) {
        switch( colour ) {
            case TERM_COLOR_BLACK:
                *color = 0x002F4F4F;
                break;
            case TERM_COLOR_RED:
                *color = 0x00FF9999;
                break;
            case TERM_COLOR_GREEN:
                *color = 0x00B3FF99;
                break;
            case TERM_COLOR_YELLOW:
                *color = 0x00FFF599;
                break;
            case TERM_COLOR_BLUE:
                *color = 0x0099D1FF;
                break;
            case TERM_COLOR_MAGENTA:
                *color = 0x00B899FF;
                break;
            case TERM_COLOR_CYAN:
                *color = 0x0099FFD6;
                break;
            case TERM_COLOR_WHITE:
                *color = 0x00FF99F0;
                break;
            default:
                return EINVAL;
        }
    } else {
        switch( colour ) {
            case TERM_COLOR_BLACK:
                *color = 0x00000000;
                break;
            case TERM_COLOR_RED:
                *color = 0x00FF3333;
                break;
            case TERM_COLOR_GREEN:
                *color = 0x0066FF33;
                break;
            case TERM_COLOR_YELLOW:
                *color = 0x00FFEB33;
                break;
            case TERM_COLOR_BLUE:
                *color = 0x0033A3FF;
                break;
            case TERM_COLOR_MAGENTA:
                *color = 0x007033FF;
                break;
            case TERM_COLOR_CYAN:
                *color = 0x0033FFAD;
                break;
            case TERM_COLOR_WHITE:
                *color = 0x00FFF8DC;
                break;
            default:
                return EINVAL;
        }    }

    return 0;
}

uint32_t term_get_fg_color(uint32_t attrib, uint32_t colour)
{
    uint32_t out;
    if( !term_get_color(attrib, (colour >> TERM_FG_SHIFT) & TERM_FG_MASK, &out) ) {
        return out;
    } else {
        return 0x00FFFFFF;
    }
}

uint32_t term_get_bg_color(uint32_t attrib, uint32_t colour)
{
    uint32_t out;
    if( !term_get_color(attrib, (colour >> TERM_BG_SHIFT) & TERM_BG_MASK, &out) ) {
        return out;
    } else {
        return 0x00000000;
    }
}

void term_scroll( term_t handle, int row )
{
    term_t_i *term;

    term = TO_S(handle);

    if( row < 0 ) {
        row = 0;
    }

    if( row > term->grid.history - term->grid.height ) {
        row = term->grid.history - term->grid.height;
    }
}

int term_resize( term_t handle, int new_width, int new_height, int new_scrollback )
{
    int ret;
    term_grid g;
    int old_crow, old_ccol;
    struct winsize ws;
    term_t_i *term = TO_S(handle);

    if( new_width == term->grid.width && new_height == term->grid.height && new_scrollback + new_height == term->grid.history) return 0;

    // Remember the previous cursor position
    old_crow = term->crow;
    old_ccol = term->ccol;

    if( term_resize_internal( handle, new_width, new_height, new_scrollback, term->extrawidth, &g ) ) {
        return 1;
    }

    ws.ws_row = new_height;
    ws.ws_col = new_width;
    ws.ws_xpixel = new_width;
    ws.ws_ypixel = new_height;

    ret = ioctl(term->fd, TIOCSWINSZ, &ws);

    if( ret != -1 ) {
        term_release_grid( &term->grid );
        memcpy( &term->grid, &g, sizeof( term_grid ) );

        // Now our internals are up-to-date, notify the application
        ret = kill(term->child, SIGWINCH);
        if( term->cursor_update != NULL ) term->cursor_update(TO_H(term), old_crow, old_ccol - (term->grid.history - term->grid.height), term->ccol, term->crow - (term->grid.history - term->grid.height));
    } else {
        term_release_grid( &g );
    }

}

int term_resize_internal( term_t handle, int new_width, int new_height, int new_scrollback, int new_extrawidth, term_grid *g )
{
    term_t_i *term;
    term_grid local_grid;
    int ret = 0;
    bool finalize;
    int width, height, offset_y_src, offset_y_dst, new_history;
    int old_crow, old_ccol;

    if( g != NULL ) {
        finalize = false;
    } else {
        finalize = true;
        g = &local_grid;
    }

    term = TO_S(handle);

    if( new_width == term->grid.width && new_height == term->grid.height && new_scrollback + new_height == term->grid.history && new_extrawidth == term->extrawidth) return 0;

    // Remember the previous cursor position
    old_crow = term->crow;
    old_ccol = term->ccol;

    // Copy the narrower of the two
    if( term->grid.width + term->extrawidth < new_width + new_extrawidth ) {
        width = term->grid.width + term->extrawidth;
    } else {
        width = new_width + new_extrawidth;
    }

    // Convenience variable
    new_history = new_height + new_scrollback;

    // We want to copy such that the first lines after the scrollback are in
    // the same position, unless that would push the cursor off the screen
    if( term->crow - (term->grid.history - term->grid.height) >= new_height ) {
        // Put the cursor at the end
        if( term->crow < new_history ) {
            offset_y_src = 0;
            offset_y_dst = new_history - term->crow;
        } else {
            offset_y_src = term->crow - new_history + 1;
            offset_y_dst = 0;
        }
        term->crow = new_history - 1;
    } else {
        // Match up the first lines
        if( term->grid.history - term->grid.height < new_scrollback ) {
            offset_y_src = 0;
            offset_y_dst = new_scrollback - (term->grid.history - term->grid.height);
        } else {
            offset_y_src = (term->grid.history - term->grid.height) - new_scrollback;
            offset_y_dst = 0;
        }
        term->crow += new_scrollback - (term->grid.history - term->grid.height);
    }

    // Figure out the height based on the later start to the earlier end
    if( term->grid.history - offset_y_src < new_history - offset_y_dst ) {
        height = term->grid.history - offset_y_src;
    } else {
        height = new_history - offset_y_dst;
    }

    g->width = new_width;
    g->height = new_height;
    g->history = new_history;

    if( !term_allocate_grid( g ) ) {
        return -1;
    }

    term_copy_grid( g, &term->grid, offset_y_src, offset_y_dst, width, height );

    if( finalize ) {
        term_release_grid( &term->grid );
        memcpy( &term->grid, g, sizeof( term_grid ) );

        // Now our internals are up-to-date, notify the application
        ret = kill(term->child, SIGWINCH);
        if( term->cursor_update != NULL ) term->cursor_update(TO_H(term), old_crow, old_ccol - (term->grid.history - term->grid.height), term->ccol, term->crow - (term->grid.history - term->grid.height));
    }

    return ret;
}

bool term_process_child(term_t handle)
{
    int length;
    char buf[100];
    term_t_i *term;

    term = TO_S(handle);

    length = read( term->fd, buf, 100 );
    if( length == -1 || length == 0) {
        errno = ECHILD;
        return false;
    }
    term_process_output_data(term, buf, length);
    return true;
}

void term_send_data(term_t handle, const char *string, int length)
{
    term_t_i *term;

    term = TO_S(handle);

#if 0
    {
        int i;
        for( i = 0; i < length; i ++ ) {
            if( isgraph( string[ i ] ) ) {
                printf( "Output character %d (%c)\n", string[ i ], string[ i ] );
            } else {
                printf( "Output character %d\n", string[ i ] );
            }
        }
    }
#endif

    write(term->fd, string, length);
}

void term_send_special(term_t handle, term_special_key key)
{
    char *sequence;
    term_t_i *term;

    term = TO_S(handle);
    switch( key ) {
        case TERM_KEY_UP:
            sequence = term_find_escape( term, escape_kcuu1 );
            break;
        case TERM_KEY_DOWN:
            sequence = term_find_escape( term, escape_kcud1 );
            break;
        case TERM_KEY_RIGHT:
            sequence = term_find_escape( term, escape_kcuf1 );
            break;
        case TERM_KEY_LEFT:
            sequence = term_find_escape( term, escape_kcub1 );
            break;
        default:
            break;
    }

    if( sequence != NULL ) {
        write( term->fd, sequence, strlen(sequence) );
    }
}

bool term_create(term_t *t)
{
    term_t_i *term = malloc(sizeof(term_t_i));

    if( term == NULL ) {
        errno = ENOMEM;
        return false;
    }

    memset( term, 0, sizeof(term_t_i) );
    term->type = TERM_TYPE_VT100;
    term->loginShell = true;

    *t = TO_H(term);
    return true;
}

int term_set_autoexpand(term_t handle, bool enabled)
{
    term_t_i *term = TO_S( handle );
    
    term->autoexpand = enabled;

    // Bring down to actual width
    if( !term->autoexpand ) {
        term->extrawidth = 0;
        term_resize_internal( handle, term->grid.width, term->grid.height, term->grid.history - term->grid.height, term->extrawidth, NULL );
    }
}

bool term_set_shell(term_t handle, char *shell)
{
    term_t_i *term = TO_S( handle );

    free( term->shell );
    term->shell = strdup( shell );
    term->loginShell = true;
    term->fork = NULL;

    if( term->shell == NULL ) {
        errno = ENOMEM;
    }

    return term->shell != NULL;
}

bool term_set_program(term_t handle, char *program)
{
    term_t_i *term = TO_S( handle );

    free( term->shell );
    term->shell = strdup( program );
    term->loginShell = false;
    term->fork = NULL;

    if( term->shell == NULL ) {
        errno = ENOMEM;
    }

    return term->shell != NULL;
}

bool term_set_fork_callback(term_t handle, int (*callback)(term_t handle, int argc, char **argv))
{
    term_t_i *term = TO_S( handle );

    term->fork = callback;
    term->loginShell = false;

    return true;
}

bool term_begin(term_t handle, int width, int height, int scrollback)
{
    term_t_i *term = TO_S(handle);

    term->grid.width = width;
    term->grid.height = height;
    term->grid.history = height + scrollback;
    term->crow = scrollback;

    if( !term_allocate_grid(&term->grid) ) {
        errno = ENOMEM;
        return false;
    }

    if( !term_fork(term) ) {
        term_release_grid(&term->grid);
        return false;
    }

    term->allocated = true;

    return true;
}

void term_free(term_t handle)
{
    term_t_i *term = TO_S(handle);

    term_slay( term );
    term_release_grid( &term->grid );
    if( term->shell != NULL ) {
        free( term->shell );
    }
    if( term->conversion_buffer != NULL ) {
        free( term->conversion_buffer );
    }
    if( term->output_bytes != NULL ) {
        free( term->output_bytes );
    }
    free( term );
}

void term_set_user_data(term_t handle, void *user_data)
{
    term_t_i *term = TO_S(handle);

    term->user_data = user_data;
}

void *term_get_user_data(term_t handle)
{
    term_t_i *term = TO_S(handle);

    return term->user_data;
}

int term_get_width(term_t handle)
{
    term_t_i *term = TO_S(handle);

    return term->grid.width;
}

int term_get_height(term_t handle)
{
    term_t_i *term = TO_S(handle);

    return term->grid.height;
}

int term_get_grid_size(term_t handle, int *w, int *h)
{
    term_t_i *term = TO_S(handle);
    *w = term->grid.width;
    *h = term->grid.height;

    return 0;
}

int term_get_cursor_pos(term_t handle, int *x, int *y)
{
    term_t_i *term = TO_S(handle);

    *x = term->ccol;
    *y = term->crow - (term->grid.history - term->grid.height);

    return 0;
}

int term_set_emulation(term_t handle, term_type type)
{
    term_t_i *term = TO_S(handle);

    if( type < 0 || type > TERM_TYPE_MAX ) {
        errno = EINVAL;
        return -1;
    }

    term->type = type;

    return 0;
}
