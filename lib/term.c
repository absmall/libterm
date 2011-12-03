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

const uint32_t **term_get_grid(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->grid + term->row;
}

const uint32_t **term_get_attribs(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->attribs + term->row;
}

const uint32_t **term_get_colours(term_t handle)
{
    term_t_i *term;

    term = TO_S(handle);

    return (const uint32_t **)term->colours + term->row;
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

    if( row > term->history - term->height ) {
        row = term->history - term->height;
    }

    term->row = row;
}

int term_resize( term_t handle, int width, int height )
{
    int ret;
    struct winsize ws;
    term_t_i *term;

    term = TO_S(handle);

    if( width == term->width && height == term->height ) return 0;

    ws.ws_row = height;
    ws.ws_col = width;
    ws.ws_xpixel = width;
    ws.ws_ypixel = height;

    term->width = width;
    term->height = height;

    term_allocate_grid(term);

    ret = ioctl(term->fd, TIOCSWINSZ, &ws);

    if( ret != -1 ) {
        ret = kill(term->child, SIGWINCH);
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

    term->width = width;
    term->height = height;
    term->history = height + scrollback;
    term->crow = scrollback;

    if( !term_allocate_grid(term) ) {
        errno = ENOMEM;
        return false;
    }

    if( !term_fork(term) ) {
        term_release_grid(term);
        return false;
    }

    return true;
}

void term_free(term_t handle)
{
    term_t_i *term = TO_S(handle);

    term_slay( term );
    term_release_grid( term );
    if( term->shell != NULL ) {
        free( term->shell );
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
