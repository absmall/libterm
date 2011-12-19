#include <stdio.h>
#include <stdlib.h>
#include "terminfo_commands.h"

// graphics charset pairs, based on vt100
void escape_acsc(term_t_i *term)
{
    fprintf(stderr, "escape_acsc unsupported!\n");
}

// audible signal (bell)
void escape_bel(term_t_i *term)
{
    if( term->bell != NULL ) term->bell(TO_H(term));
}

// turn on blinking
void escape_blink(term_t_i *term)
{
    term->cattr |= TERM_ATTRIB_BLINK;
}

// turn on bold (extra bright) mode
void escape_bold(term_t_i *term)
{
    term->cattr |= TERM_ATTRIB_BOLD;
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
    term_add_dirty_rect( term, 0, 0, term->grid.width, term->grid.height );
    term->dirty_cursor.exists = true;
}

// carriage return
void escape_cr(term_t_i *term)
{
    term->ccol = 0;
}

// Change scrolling region
void escape_csr(term_t_i *term)
{
    fprintf(stderr, "escape_csr unsupported!\n");
}

// Move cursor left #1 spaces
void escape_cub(term_t_i *term)
{
    int left = atoi( term->output_bytes + 2 );
    if( term->ccol < left ) {
        term->ccol = 0;
    }  else {
        term->ccol -= left;
    }
    term->dirty_cursor.exists = 1;
}

// move left one space
void escape_cub1(term_t_i *term)
{
    if( term->ccol > 0 ) {
        term->ccol --;
        term->dirty_cursor.exists = true;
    }
}

// Move down #1 lines
void escape_cud(term_t_i *term)
{
    int down = atoi( term->output_bytes + 2 );
    if( term->crow + down >= term->grid.history ) {
        term->crow = term->grid.history - 1;
    }  else {
        term->crow += down;
    }
    term->dirty_cursor.exists = true;
}

// down one line
void escape_cud1(term_t_i *term)
{
    term->ccol=0;
    term->crow++;
    term->dirty_cursor.exists = true;
    if( term->crow >= term->grid.history ) {
        term_shiftrows(term);
        term_add_dirty_rect( term, 0, 0, term->grid.width, term->grid.height );
    }
}

// Move right #1 spaces
void escape_cuf(term_t_i *term)
{
    int right = atoi( term->output_bytes + 2 );
    if( term->ccol + right >= term->grid.width ) {
        term->ccol = term->grid.width - 1;
    }  else {
        term->ccol += right;
    }
    term->dirty_cursor.exists = true;
}

// move right one space
void escape_cuf1(term_t_i *term)
{
    if( term->ccol + 1 >= term->grid.width ) {
        term->ccol = term->grid.width - 1;
    }  else {
        term->ccol ++;
    }
    term->dirty_cursor.exists = true;
}

// Move to row #1 col #2
void escape_cup(term_t_i *term)
{
    char *n;

    term->crow = strtoul( term->output_bytes + 2, &n, 10 ) - 1;
    if( term->crow >= term->grid.height ) term->crow = term->grid.height - 1;
    term->ccol = strtoul( n + 1, NULL, 10 ) - 1;
    if( term->ccol >= term->grid.width ) term->crow = term->grid.width - 1;
    term->dirty_cursor.exists = true;
}

// Move cursor up #1 lines
void escape_cuu(term_t_i *term)
{
    int up = atoi( term->output_bytes + 2 );
    if( term->crow < up ) {
        term->crow = 0;
    }  else {
        term->crow -= up;
    }
    term->dirty_cursor.exists = true;
}

// up one line
void escape_cuu1(term_t_i *term)
{
    if( term->crow > 0 ) {
        term->crow --;
        term->dirty_cursor.exists = true;
    }
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
    term_add_dirty_rect( term, term->ccol, term->crow, term->grid.width - term->ccol, term->grid.height - term->crow );
}

// Clear to end of line
void escape_el(term_t_i *term)
{
    int i;
    for( i = term->ccol; i < term->grid.width; i ++ ) {
        term->grid.grid[ term->crow ][ i ] = ' ';
    }
    term_add_dirty_rect( term, term->ccol, term->crow, term->grid.width - term->ccol, 1 );
}

// Clear to beginning of line
void escape_el1(term_t_i *term)
{
    fprintf(stderr, "escape_el1 unsupported!\n");
}

// enable alternate char set
void escape_enacs(term_t_i *term)
{
    fprintf(stderr, "escape_enacs unsupported!\n");
}

// Home cursor
void escape_home(term_t_i *term)
{
    term->crow = term->grid.history - term->grid.height;
    term->ccol = 0;
    term->dirty_cursor.exists = true;
}

// tab to next 8-space hardware tab stop
void escape_ht(term_t_i *term)
{
    fprintf(stderr, "escape_ht unsupported!\n");
}

// set a tab in every row, current columns
void escape_hts(term_t_i *term)
{
    fprintf(stderr, "escape_hts unsupported!\n");
}

// scroll text up
void escape_ind(term_t_i *term)
{
    fprintf(stderr, "escape_ind unsupported!\n");
}

// upper left of keypad
void escape_ka1(term_t_i *term)
{
    fprintf(stderr, "escape_ka1 unsupported!\n");
}

// upper right of keypad
void escape_ka3(term_t_i *term)
{
    fprintf(stderr, "escape_ka3 unsupported!\n");
}

// center of keypad
void escape_kb2(term_t_i *term)
{
    fprintf(stderr, "escape_kb2 unsupported!\n");
}

// backspace key
void escape_kbs(term_t_i *term)
{
    fprintf(stderr, "escape_kbs unsupported!\n");
}

// lower left of keypad
void escape_kc1(term_t_i *term)
{
    fprintf(stderr, "escape_kc1 unsupported!\n");
}

// lower right of keypad
void escape_kc3(term_t_i *term)
{
    fprintf(stderr, "escape_kc3 unsupported!\n");
}

// left-arrow key
void escape_kcub1(term_t_i *term)
{
    fprintf(stderr, "escape_kcub1 unsupported!\n");
}

// down-arrow key
void escape_kcud1(term_t_i *term)
{
    fprintf(stderr, "escape_kcud1 unsupported!\n");
}

// right-arrow key
void escape_kcuf1(term_t_i *term)
{
    fprintf(stderr, "escape_kcuf1 unsupported!\n");
}

// up-arrow key
void escape_kcuu1(term_t_i *term)
{
    fprintf(stderr, "escape_kcuu1 unsupported!\n");
}

// enter/send key
void escape_kent(term_t_i *term)
{
    fprintf(stderr, "escape_kent unsupported!\n");
}

// F0 function key
void escape_kf0(term_t_i *term)
{
    fprintf(stderr, "escape_kf0 unsupported!\n");
}

// F1 function key
void escape_kf1(term_t_i *term)
{
    fprintf(stderr, "escape_kf1 unsupported!\n");
}

// F10 function key
void escape_kf10(term_t_i *term)
{
    fprintf(stderr, "escape_kf10 unsupported!\n");
}

// F2 function key
void escape_kf2(term_t_i *term)
{
    fprintf(stderr, "escape_kf2 unsupported!\n");
}

// F3 function key
void escape_kf3(term_t_i *term)
{
    fprintf(stderr, "escape_kf3 unsupported!\n");
}

// F4 function key
void escape_kf4(term_t_i *term)
{
    fprintf(stderr, "escape_kf4 unsupported!\n");
}

// F5 function key
void escape_kf5(term_t_i *term)
{
    fprintf(stderr, "escape_kf5 unsupported!\n");
}

// F6 function key
void escape_kf6(term_t_i *term)
{
    fprintf(stderr, "escape_kf6 unsupported!\n");
}

// F7 function key
void escape_kf7(term_t_i *term)
{
    fprintf(stderr, "escape_kf7 unsupported!\n");
}

// F8 function key
void escape_kf8(term_t_i *term)
{
    fprintf(stderr, "escape_kf8 unsupported!\n");
}

// F9 function key
void escape_kf9(term_t_i *term)
{
    fprintf(stderr, "escape_kf9 unsupported!\n");
}

// label on function key f1 if not f1
void escape_lf1(term_t_i *term)
{
    fprintf(stderr, "escape_lf1 unsupported!\n");
}

// label on function key f2 if not f2
void escape_lf2(term_t_i *term)
{
    fprintf(stderr, "escape_lf2 unsupported!\n");
}

// label on function key f3 if not f3
void escape_lf3(term_t_i *term)
{
    fprintf(stderr, "escape_lf3 unsupported!\n");
}

// label on function key f4 if not f4
void escape_lf4(term_t_i *term)
{
    fprintf(stderr, "escape_lf4 unsupported!\n");
}

// print contents of screen
void escape_mc0(term_t_i *term)
{
    fprintf(stderr, "escape_mc0 unsupported!\n");
}

// turn off printer
void escape_mc4(term_t_i *term)
{
    fprintf(stderr, "escape_mc4 unsupported!\n");
}

// turn on printer
void escape_mc5(term_t_i *term)
{
    fprintf(stderr, "escape_mc5 unsupported!\n");
}

// Restore cursor to position of last sc
void escape_rc(term_t_i *term)
{
    term->crow = term->csavedrow;
    term->ccol = term->csavedcol;
    term->dirty_cursor.exists = true;
}

// turn on reverse video mode
void escape_rev(term_t_i *term)
{
    term->cattr |= TERM_ATTRIB_REVERSE;
}

// scroll text down
void escape_ri(term_t_i *term)
{
    fprintf(stderr, "escape_ri unsupported!\n");
}

// end alternate character set
void escape_rmacs(term_t_i *term)
{
    fprintf(stderr, "escape_rmacs unsupported!\n");
}

// turn off automatic margins
void escape_rmam(term_t_i *term)
{
    fprintf(stderr, "escape_rmam unsupported!\n");
}

// Out of "keypad-transmit" mode
void escape_rmkx(term_t_i *term)
{
    fprintf(stderr, "escape_rmkx unsupported!\n");
}

// exit standout mode
void escape_rmso(term_t_i *term)
{
    fprintf(stderr, "escape_rmso unsupported!\n");
}

// End underscore mode
void escape_rmul(term_t_i *term)
{
    term->cattr &= ~TERM_ATTRIB_UNDERSCORE;
}

// reset string
void escape_rs2(term_t_i *term)
{
    fprintf(stderr, "escape_rs2 unsupported!\n");
}

// Save cursor position
void escape_sc(term_t_i *term)
{
    term->csavedrow = term->crow;
    term->csavedcol = term->ccol;
}

// define video attributes #1-#9
void escape_sgr(term_t_i *term)
{
    fprintf(stderr, "escape_sgr unsupported!\n");
}

// Turn off all attributes
void escape_sgr0(term_t_i *term)
{
    term->cattr = 0;
    term->ccolour = 0;
}

// start alternate character set
void escape_smacs(term_t_i *term)
{
    fprintf(stderr, "escape_smacs unsupported!\n");
}

// turn on automatic margins
void escape_smam(term_t_i *term)
{
    fprintf(stderr, "escape_smam unsupported!\n");
}

// Begin keypad transmit mode.
void escape_smkx(term_t_i *term)
{
    fprintf(stderr, "escape_smkx unsupported!\n");
}

// begin standout mode
void escape_smso(term_t_i *term)
{
    fprintf(stderr, "escape_smso unsupported!\n");
}

// begin underline mode
void escape_smul(term_t_i *term)
{
    term->cattr |= TERM_ATTRIB_UNDERSCORE;
}

// clear all tab stops
void escape_tbc(term_t_i *term)
{
    fprintf(stderr, "escape_tbc unsupported!\n");
}

void escape_sgm(term_t_i *term)
{
    int i = 2;
    int val;
    char *nptr;

    while( true ) {
        val = strtoul( term->output_bytes + i, &nptr, 10 );
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
        i = nptr - term->output_bytes + 1;
    }
}
