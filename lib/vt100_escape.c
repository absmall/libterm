#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libterm_internal.h>

void escape_EL0(term_t_i *term)
{
    int i;
    for( i = term->ccol; i < term->width; i ++ ) {
        term->grid[ term->crow ][ i ] = ' ';
    }
}

void escape_SGR0(term_t_i *term)
{
    term->cattr = 0;
    term->ccolour = 0;
}

int match_sgm(term_t_i *term, int *length)
{
    int i;
    int state = 0;

    for( i = 0; i < term->escape_bytes; i ++ ) {
        if( i == 0 ) {
            if( term->escape_code[ i ] != 27 ) return 0;
        } else if( i == 1 ) {
            if( term->escape_code[ i ] != '[' ) {
                return 0;
            } else {
                state ++;
            }
        } else if( state == 1 ) {
            if( term->escape_code[ i ] == 'm'
             && isdigit( term->escape_code[ i - 1 ] ) ) {
                *length = i + 1;
                return 2;
            }
            else if( !isdigit( term->escape_code[ i ] )
             && term->escape_code[ i ] != ';' ) return 0;
        }
    }

    return 1;
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

struct static_escape_code {
    char *code;
    void (*apply_escape_code)(term_t_i *term);
} static_escape_codes [] = {
    { "\x1b[K", escape_EL0 },
    { "\x1b[0m", escape_SGR0 }
#if 0
    { "\x1b[0d", "???" },
    { "\x1b[01;d", "???" 
#endif
};

struct dynamic_escape_code {
    int (*code_match)(term_t_i *term, int *length);
    void (*apply_escape_code)(term_t_i *term);
} dynamic_escape_codes[] = {
    { match_sgm, escape_sgm }
};

int term_send_escape(term_t_i *term, char *buf, int length)
{
    bool isprefix = false;
    int previous_length;
    int i;

    // See if we need to reallocate
    if( term->escape_max_bytes < term->escape_bytes + length ) {
        term->escape_max_bytes = term->escape_bytes + length;
        if( term->escape_code != NULL ) {
            term->escape_code = realloc( term->escape_code, term->escape_max_bytes );
        } else {
            term->escape_code = malloc( term->escape_max_bytes );
        }
    }
    memcpy( term->escape_code + term->escape_bytes, buf, length ); 
    previous_length = term->escape_bytes;
    term->escape_bytes += length;
    
    // See if this is a prefix, or is equal to any static escape_code
    for( i = 0; i < sizeof(static_escape_codes)/sizeof(struct static_escape_code); i ++ ) {
        int codelen = strlen( static_escape_codes[ i ].code );
        if( term->escape_bytes < codelen ) {
            if( strncmp(static_escape_codes[ i ].code, term->escape_code, term->escape_bytes ) == 0 ) {
                isprefix = true;
            }
        } else if( strncmp(static_escape_codes[ i ].code, term->escape_code, codelen ) == 0 ) {
            static_escape_codes[ i ].apply_escape_code( term );
            term->escape_bytes = 0;
            term->escape_mode = 0;
            return codelen - previous_length;
        }
    }

    // See if this is a prefix, or is equal to any dynamic escape_code
    for( i = 0; i < sizeof(dynamic_escape_codes)/sizeof(struct dynamic_escape_code); i ++ ) {
        int ret;
        int codelen;
        ret = dynamic_escape_codes[ i ].code_match( term, &codelen );
        if( ret == 2 ) {
            dynamic_escape_codes[ i ].apply_escape_code( term );
            term->escape_bytes = 0;
            term->escape_mode = 0;
            return codelen - previous_length;
        } else if( ret == 1 ) {
            isprefix = true;
        }
    }

    if( isprefix ) {
        // We ate everything
        return length;
    } else {
        // It's a false escape code - consume a byte so that we won't try to
        // process this again, and discard whatever we have accumulated
        printf( "Unknown escape code:" );
        for(i = 0; i < term->escape_bytes; i ++ ) {
            printf( " %d", term->escape_code[ i ] );
        }
        printf( "\n" );
        term->escape_bytes = 0;
        term->escape_mode = 0;
        return 1;
    }
}
