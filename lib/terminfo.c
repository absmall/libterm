#include <stdlib.h>
#include "terminfo_commands.h"

// Move to row #1 col #2
void escape_cup(term_t_i *term)
{
    char *n;

    term->crow = strtoul( term->escape_code + 2, &n, 10 ) - 1;
    if( term->crow >= term->grid.height ) term->crow = term->grid.height - 1;
    term->ccol = strtoul( n + 1, NULL, 10 ) - 1;
    if( term->ccol >= term->grid.width ) term->crow = term->grid.width - 1;
}

// Move cursor up #1 lines
void escape_cuu(term_t_i *term)
{
    int up = atoi( term->escape_code + 2 );
    if( term->crow < up ) {
        term->crow = 0;
    }  else {
        term->crow -= up;
    }
}

// Move down #1 lines
void escape_cud(term_t_i *term)
{
    int down = atoi( term->escape_code + 2 );
    if( term->crow + down >= term->grid.history ) {
        term->crow = term->grid.history - 1;
    }  else {
        term->crow += down;
    }
}

// Move right #1 spaces
void escape_cuf(term_t_i *term)
{
    int right = atoi( term->escape_code + 2 );
    if( term->ccol + right >= term->grid.width ) {
        term->ccol = term->grid.width - 1;
    }  else {
        term->ccol += right;
    }
}

// Move cursor left #1 spaces
void escape_cub(term_t_i *term)
{
    int left = atoi( term->escape_code + 2 );
    if( term->ccol < left ) {
        term->ccol = 0;
    }  else {
        term->ccol -= left;
    }
}

// Save cursor position
void escape_sc(term_t_i *term)
{
    term->csavedrow = term->crow;
    term->csavedcol = term->ccol;
}

// Restore cursor to position of last sc
void escape_rc(term_t_i *term)
{
    term->crow = term->csavedrow;
    term->ccol = term->csavedcol;
}

// Clear to end of line
void escape_el(term_t_i *term)
{
    int i;
    for( i = term->ccol; i < term->grid.width; i ++ ) {
        term->grid.grid[ term->crow ][ i ] = ' ';
    }
}

// Clear screen and home cursor
void escape_clear(term_t_i *term)
{
    int i, j;
    for( i = 0; i < term->grid.height; i ++ ) {
        for( j = 0; j < term->grid.width; j ++ ) {
            term->grid.grid[ i ][ j ] = ' ';
            term->grid.attribs[ i ][ j ] = 0;
        }
    }
    term->crow = term->grid.history - term->grid.height;
    term->ccol = 0;
}

// Turn off all attributes
void escape_sgr0(term_t_i *term)
{
    term->cattr = 0;
    term->ccolour = 0;
}

// Begin keypad transmit mode.
void escape_smkx(term_t_i *term)
{
    // FIXME
}

// Change scrolling region
void escape_csr(term_t_i *term)
{
    // FIXME
}

// Home cursor
void escape_home(term_t_i *term)
{
    term->crow = term->grid.history - term->grid.height;
    term->ccol = 0;
}

// End underscore mode
void escape_rmul(term_t_i *term)
{
    term->cattr &= ~TERM_ATTRIB_UNDERSCORE;
}

// Clear to end of display
void escape_ed(term_t_i *term)
{
    int i, j;
    for( i = term->crow; i < term->grid.height; i ++ ) {
        for( j = term->ccol; j < term->grid.width; j ++ ) {
            term->grid.grid[ i ][ j ] = ' ';
            term->grid.attribs[ i ][ j ] = 0;
        }
    }
}

// Out of "keypad-transmit" mode
void escape_rmkx(term_t_i *term)
{
    // FIXME
}

void escape_cuf1(term_t_i *term)
{
    if( term->ccol + 1 >= term->grid.width ) {
        term->ccol = term->grid.width - 1;
    }  else {
        term->ccol ++;
    }
}

void escape_sgm(term_t_i *term)
{
    int i = 2;
    int val;
    char *nptr;

    while( true ) {
        val = strtoul( term->escape_code + i, &nptr, 10 );
        switch( val ) {
            case 0:
                term->cattr = 0;
                break;
            case 1:
                term->cattr |= TERM_ATTRIB_BOLD;
                break;
            case 4:
                term->cattr |= TERM_ATTRIB_UNDERSCORE;
                break;
            case 5:
                term->cattr |= TERM_ATTRIB_BLINK;
                break;
            case 7:
                term->cattr |= TERM_ATTRIB_REVERSE;
                break;
            case 8:
                term->cattr |= TERM_ATTRIB_CONCEALED;
                break;
            case 30:
                // Black
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_BLACK << TERM_FG_SHIFT;
                break;
            case 31:
                // Red
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_RED << TERM_FG_SHIFT;
                break;
            case 32:
                // Green
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_GREEN << TERM_FG_SHIFT;
                break;
            case 33:
                // Yellow
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_YELLOW << TERM_FG_SHIFT;
                break;
            case 34:
                // Blue
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_BLUE << TERM_FG_SHIFT;
                break;
            case 35:
                // Magenta
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_MAGENTA << TERM_FG_SHIFT;
                break;
            case 36:
                // Cyan
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_CYAN << TERM_FG_SHIFT;
                break;
            case 37:
                // White
                term->ccolour &= ~TERM_FG_MASK;
                term->ccolour |= TERM_COLOR_WHITE << TERM_FG_SHIFT;
                break;
            case 40:
                // Black
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_BLACK << TERM_BG_SHIFT;
                break;
            case 41:
                // Red
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_RED << TERM_BG_SHIFT;
                break;
            case 42:
                // Green
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_GREEN << TERM_BG_SHIFT;
                break;
            case 43:
                // Yellow
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_YELLOW << TERM_BG_SHIFT;
                break;
            case 44:
                // Blue
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_BLUE << TERM_BG_SHIFT;
                break;
            case 45:
                // Magenta
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_MAGENTA << TERM_BG_SHIFT;
                break;
            case 46:
                // Cyan
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_CYAN << TERM_BG_SHIFT;
                break;
            case 47:
                // White
                term->ccolour &= ~TERM_BG_MASK;
                term->ccolour |= TERM_COLOR_WHITE << TERM_BG_SHIFT;
                break;
        }
        if( (*nptr) == 'm' ) break;
        i = nptr - term->escape_code + 1;
    }
}
