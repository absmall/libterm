#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libterm_internal.h>

static int escape_compare( char *code, char *data, int datalen, int *retlen )
{
    typedef enum {
        CHAR,
        STARTNUMBER,
        NUMBER
    } Mode;
    Mode m = CHAR;
    int i, j, codelen;

    codelen = strlen(code);
    for( i = 0, j = 0; i < codelen && j < datalen; ) {
        if( m == CHAR ) {
            if( code[ i ] == '%' ) {
                i ++;
                if( code[ i ] == 'd' ) {
                    m = STARTNUMBER;
                }
                i ++;
            } else if( code[ i ] == data[ j ] ) {
                i ++;
                j ++;
            } else {
                return 0;
            }
        } else if( m == STARTNUMBER ) {
            if( isdigit( data[ j ] ) ) {
                j ++;
                m = NUMBER;
            } else {
                return 0;
            }
        } else if( m == NUMBER ) {
            if( isdigit( data[ j ] ) ) {
                j ++;
            } else {
                m = CHAR;
            }
        }
    }

    if( i == codelen ) {
        *retlen = j;
        return 2;
    } else {
        return 1;
    }
}

void escape_cup(term_t_i *term)
{
    char *n;

    term->crow = strtoul( term->escape_code + 2, &n, 10 );
    term->ccol = strtoul( n + 1, NULL, 10 );
}

void escape_cuu(term_t_i *term)
{
    int up = atoi( term->escape_code + 2 );
    if( term->crow < up ) {
        term->crow = 0;
    }  else {
        term->crow -= up;
    }
}

void escape_cud(term_t_i *term)
{
    int down = atoi( term->escape_code + 2 );
    if( term->crow + down >= term->history ) {
        term->crow = term->history - 1;
    }  else {
        term->crow += down;
    }
}

void escape_cuf(term_t_i *term)
{
    int right = atoi( term->escape_code + 2 );
    if( term->ccol + right >= term->width ) {
        term->ccol = term->width - 1;
    }  else {
        term->ccol += right;
    }
}

void escape_cub(term_t_i *term)
{
    int left = atoi( term->escape_code + 2 );
    if( term->ccol < left ) {
        term->ccol = 0;
    }  else {
        term->ccol -= left;
    }
}

void escape_SCP(term_t_i *term)
{
    term->csavedrow = term->crow;
    term->csavedcol = term->ccol;
}

void escape_RCP(term_t_i *term)
{
    term->crow = term->csavedrow;
    term->ccol = term->csavedcol;
}

void escape_el(term_t_i *term)
{
    int i;
    for( i = term->ccol; i < term->width; i ++ ) {
        term->grid[ term->crow ][ i ] = ' ';
    }
}

void escape_clear(term_t_i *term)
{
    int i, j;
    for( i = 0; i < term->height; i ++ ) {
        for( j = 0; j < term->width; j ++ ) {
            term->grid[ i ][ j ] = ' ';
            term->attribs[ i ][ j ] = 0;
        }
    }
    term->ccol = 0;
    term->crow = 0;
}

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
    // FIXME
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
    { "\x1b[%d;%dH", escape_cup },
    { "\x1b[%d;%df", escape_cup },
    { "\x1b[%dA", escape_cuu },
    { "\x1b[%dB", escape_cud },
    { "\x1b[%dC", escape_cuf },
    { "\x1b[%dD", escape_cub },
    { "\x1b[2J", escape_clear },
    { "\x1b[K", escape_el },
    { "\x1b[0m", escape_sgr0 },
    { "\x1b[?1h\x1b", escape_smkx },
    { "\x1b[%d;%dr", escape_csr },
    { "\x1b[H", escape_home },
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
        int codelen;
        int ret = escape_compare( static_escape_codes[ i ].code, term->escape_code, term->escape_bytes, &codelen );
        if( ret == 2 ) {
            static_escape_codes[ i ].apply_escape_code( term );
            term->escape_bytes = 0;
            term->escape_mode = 0;
            return codelen - previous_length;
        } else if( ret == 1 ) {
            isprefix = true;
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
            if( isprint(term->escape_code[ i ] ) ) {
                printf( " %c", term->escape_code[ i ] );
            } else {
                printf( " 0x%02X", term->escape_code[ i ] );
            }
        }
        printf( "\n" );
        term->escape_bytes = 0;
        term->escape_mode = 0;
        return 1;
    }
}
