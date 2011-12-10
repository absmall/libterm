#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "libterm_internal.h"

void term_register_update(term_t handle, void (*update)(term_t handle, int x, int y, int width, int height))
{
    term_t_i *term;

    term = TO_S(handle);

    term->update = update;
}

void term_register_cursor(term_t handle, void (*update)(term_t handle, int x, int y))
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

    return (const wchar_t **)term->grid.grid + term->row;
}

const uint32_t **term_get_attribs(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->grid.attribs + term->row;
}

const uint32_t **term_get_colours(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->grid.colours + term->row;
}

const char *term_get_line(term_t handle, int row)
{
    unsigned int length;
    term_t_i *term;
    wchar_t *grid_row;

    term = TO_S(handle);

    grid_row = term->grid.grid[term->row + row];
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

    term->row = row;
    
    if( row < 0 ) {
        row = 0;
    }

    if( row > term->grid.history - term->grid.height ) {
        row = term->grid.history - term->grid.height;
    }

    term->row = row;
}

int term_resize( term_t handle, int new_width, int new_height, int new_scrollback )
{
    int ret;
    struct winsize ws;
    term_t_i *term;
    term_grid g;
    int width, height, offset_y_src, offset_y_dst, new_history;

    term = TO_S(handle);

    if( new_width == term->grid.width && new_height == term->grid.height && new_scrollback + new_height == term->grid.history) return 0;

    // Copy the narrower of the two
    if( term->grid.width < new_width ) {
        width = term->grid.width;
    } else {
        width = new_width;
    }

    // Convenience variable
    new_history = new_height + new_scrollback;

    // We want to copy such that the first lines after the scrollback are in
    // the same position, unless that would push the cursor off the screen
    if( term->crow >= new_height ) {
        // Put the cursor at the end
        if( term->crow < new_history ) {
            offset_y_src = 0;
            offset_y_dst = new_history - term->crow;
        } else {
            offset_y_src = term->crow - new_history;
            offset_y_dst = 0;
        }
        term->crow = new_height - 1;
    } else {
        // Match up the first lines
        if( term->grid.history - term->grid.height < new_scrollback ) {
            offset_y_src = 0;
            offset_y_dst = new_scrollback - (term->grid.history - term->grid.height);
        } else {
            offset_y_src = (term->grid.history - term->grid.height) - new_scrollback;
            offset_y_dst = 0;
        }
        term->crow += (term->grid.history - term->grid.height) - new_scrollback;
    }

    // Figure out the height based on the later start to the earlier end
    if( term->grid.history - offset_y_src < new_history - offset_y_dst ) {
        height = term->grid.history - offset_y_src;
    } else {
        height = new_history - offset_y_dst;
    }

    ws.ws_row = new_height;
    ws.ws_col = new_width;
    ws.ws_xpixel = new_width;
    ws.ws_ypixel = new_height;

    g.width = new_width;
    g.height = new_height;
    g.history = new_history;

    if( !term_allocate_grid( &g ) ) {
        return -1;
    }

    term_copy_grid( &g, &term->grid, offset_y_src, offset_y_dst, width, height );

    ret = ioctl(term->fd, TIOCSWINSZ, &ws);

    if( ret != -1 ) {
#ifndef __QNX__
        // I'm not sure why ccol needs to be set on linux, but thisseems to be
        // necessary before sending SIGWINCH, because bash responds by
        // re-outputting the current line. The qnx shell does not do the same
        // thing
        term->ccol = 0;
#endif
        ret = kill(term->child, SIGWINCH);
        term_release_grid( &term->grid );
        memcpy( &term->grid, &g, sizeof( term_grid ) );
    } else {
        term_release_grid( &g );
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
    if( length == -1 ) {
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
    term_t_i *term;

    term = TO_S(handle);
    switch( key ) {
        case TERM_KEY_UP:
            write( term->fd, "\x1b[A", 3);
            break;
        case TERM_KEY_DOWN:
            write( term->fd, "\x1b[B", 3);
            break;
        case TERM_KEY_RIGHT:
            write( term->fd, "\x1b[C", 3);
            break;
        case TERM_KEY_LEFT:
            write( term->fd, "\x1b[D", 3);
            break;
        default:
            break;
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

    *t = TO_H(term);
    return true;
}

bool term_set_shell(term_t handle, char *shell)
{
    term_t_i *term = TO_S( handle );

    term->shell = strdup( shell );

    if( term->shell == NULL ) {
        errno = ENOMEM;
    }

    return term->shell != NULL;
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
    if( term->escape_code != NULL ) {
        free( term->escape_code );
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

void term_get_grid_size(term_t handle, int *w, int *h)
{
    term_t_i *term = TO_S(handle);
    *w = term->grid.width;
    *h = term->grid.height;
}
