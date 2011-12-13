#include <stdio.h>
#include <stdlib.h>
#include "terminfo_commands.h"

void escape_acsc(term_t_i *term)
{
    fprintf(stderr, "escape_acsc unsupported!\n");
}

void escape_bel(term_t_i *term)
{
    if( term->bell != NULL ) term->bell(TO_H(term));
}

void escape_blink(term_t_i *term)
{
    term->cattr |= TERM_ATTRIB_BLINK;
}

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
}

void escape_cr(term_t_i *term)
{
    fprintf(stderr, "escape_cr unsupported!\n");
}

// Change scrolling region
void escape_csr(term_t_i *term)
{
    fprintf(stderr, "escape_csr unsupported!\n");
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

void escape_cub1(term_t_i *term)
{
    if( term->ccol > 0 ) {
        term->ccol --;
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

void escape_cud1(term_t_i *term)
{
    fprintf(stderr, "escape_cud1 unsupported!\n");
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

void escape_cuf1(term_t_i *term)
{
    if( term->ccol + 1 >= term->grid.width ) {
        term->ccol = term->grid.width - 1;
    }  else {
        term->ccol ++;
    }
}

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

void escape_cuu1(term_t_i *term)
{
    if( term->crow > 0 ) {
        term->crow --;
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
}

// Clear to end of line
void escape_el(term_t_i *term)
{
    int i;
    for( i = term->ccol; i < term->grid.width; i ++ ) {
        term->grid.grid[ term->crow ][ i ] = ' ';
    }
}

void escape_el1(term_t_i *term)
{
    fprintf(stderr, "escape_el1 unsupported!\n");
}


void escape_enacs(term_t_i *term)
{
    fprintf(stderr, "escape_enacs unsupported!\n");
}

// Home cursor
void escape_home(term_t_i *term)
{
    term->crow = term->grid.history - term->grid.height;
    term->ccol = 0;
}

void escape_ht(term_t_i *term)
{
    fprintf(stderr, "escape_ht unsupported!\n");
}

void escape_hts(term_t_i *term)
{
    fprintf(stderr, "escape_hts unsupported!\n");
}

void escape_ind(term_t_i *term)
{
    fprintf(stderr, "escape_ind unsupported!\n");
}

void escape_ka1(term_t_i *term)
{
    fprintf(stderr, "escape_ka1 unsupported!\n");
}

void escape_ka3(term_t_i *term)
{
    fprintf(stderr, "escape_ka3 unsupported!\n");
}

void escape_kb2(term_t_i *term)
{
    fprintf(stderr, "escape_kb2 unsupported!\n");
}

void escape_kbs(term_t_i *term)
{
    fprintf(stderr, "escape_kbs unsupported!\n");
}

void escape_kc1(term_t_i *term)
{
    fprintf(stderr, "escape_kc1 unsupported!\n");
}

void escape_kc3(term_t_i *term)
{
    fprintf(stderr, "escape_kc3 unsupported!\n");
}

void escape_kcub1(term_t_i *term)
{
    fprintf(stderr, "escape_kcub1 unsupported!\n");
}

void escape_kcud1(term_t_i *term)
{
    fprintf(stderr, "escape_kcud1 unsupported!\n");
}

void escape_kcuf1(term_t_i *term)
{
    fprintf(stderr, "escape_kcuf1 unsupported!\n");
}

void escape_kcuu1(term_t_i *term)
{
    fprintf(stderr, "escape_kcuu1 unsupported!\n");
}

void escape_kent(term_t_i *term)
{
    fprintf(stderr, "escape_kent unsupported!\n");
}

void escape_kf0(term_t_i *term)
{
    fprintf(stderr, "escape_kf0 unsupported!\n");
}

void escape_kf1(term_t_i *term)
{
    fprintf(stderr, "escape_kf1 unsupported!\n");
}

void escape_kf10(term_t_i *term)
{
    fprintf(stderr, "escape_kf10 unsupported!\n");
}

void escape_kf2(term_t_i *term)
{
    fprintf(stderr, "escape_kf2 unsupported!\n");
}

void escape_kf3(term_t_i *term)
{
    fprintf(stderr, "escape_kf3 unsupported!\n");
}

void escape_kf4(term_t_i *term)
{
    fprintf(stderr, "escape_kf4 unsupported!\n");
}

void escape_kf5(term_t_i *term)
{
    fprintf(stderr, "escape_kf5 unsupported!\n");
}

void escape_kf6(term_t_i *term)
{
    fprintf(stderr, "escape_kf6 unsupported!\n");
}

void escape_kf7(term_t_i *term)
{
    fprintf(stderr, "escape_kf7 unsupported!\n");
}

void escape_kf8(term_t_i *term)
{
    fprintf(stderr, "escape_kf8 unsupported!\n");
}

void escape_kf9(term_t_i *term)
{
    fprintf(stderr, "escape_kf9 unsupported!\n");
}

void escape_lf1(term_t_i *term)
{
    fprintf(stderr, "escape_lf1 unsupported!\n");
}

void escape_lf2(term_t_i *term)
{
    fprintf(stderr, "escape_lf2 unsupported!\n");
}

void escape_lf3(term_t_i *term)
{
    fprintf(stderr, "escape_lf3 unsupported!\n");
}

void escape_lf4(term_t_i *term)
{
    fprintf(stderr, "escape_lf4 unsupported!\n");
}

void escape_mc0(term_t_i *term)
{
    fprintf(stderr, "escape_mc0 unsupported!\n");
}

void escape_mc4(term_t_i *term)
{
    fprintf(stderr, "escape_mc4 unsupported!\n");
}

void escape_mc5(term_t_i *term)
{
    fprintf(stderr, "escape_mc5 unsupported!\n");
}

// Restore cursor to position of last sc
void escape_rc(term_t_i *term)
{
    term->crow = term->csavedrow;
    term->ccol = term->csavedcol;
}

void escape_rev(term_t_i *term)
{
    fprintf(stderr, "escape_rev unsupported!\n");
}

void escape_ri(term_t_i *term)
{
    fprintf(stderr, "escape_ri unsupported!\n");
}

void escape_rmacs(term_t_i *term)
{
    fprintf(stderr, "escape_rmacs unsupported!\n");
}

void escape_rmam(term_t_i *term)
{
    fprintf(stderr, "escape_rmam unsupported!\n");
}

// Out of "keypad-transmit" mode
void escape_rmkx(term_t_i *term)
{
    fprintf(stderr, "escape_rmkx unsupported!\n");
}

void escape_rmso(term_t_i *term)
{
    fprintf(stderr, "escape_rmso unsupported!\n");
}

// End underscore mode
void escape_rmul(term_t_i *term)
{
    term->cattr &= ~TERM_ATTRIB_UNDERSCORE;
}

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

void escape_smacs(term_t_i *term)
{
    fprintf(stderr, "escape_smacs unsupported!\n");
}

void escape_smam(term_t_i *term)
{
    fprintf(stderr, "escape_smam unsupported!\n");
}

// Begin keypad transmit mode.
void escape_smkx(term_t_i *term)
{
    fprintf(stderr, "escape_smkx unsupported!\n");
}

void escape_smso(term_t_i *term)
{
    fprintf(stderr, "escape_smso unsupported!\n");
}

void escape_smul(term_t_i *term)
{
    fprintf(stderr, "escape_smul unsupported!\n");
}

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
