#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <libterm_internal.h>
#include <terminfo_commands.h>

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

struct static_escape_code {
    char *code;
    void (*apply_escape_code)(term_t_i *term);
};

struct static_escape_code escape_vt100 [] = {
    // bel
    { "\x1b""5m", escape_blink },
    { "\x1b""1m", escape_bold },
    { "\x1b[2J", escape_clear },
    // cr
    { "\x1b[%d;%dr", escape_csr },
    { "\x1b[%dD", escape_cub },
    // cub1
    { "\x1b[%dB", escape_cud },
    // cud1
    { "\x1b[%dC", escape_cuf },
    { "\x1b[C", escape_cuf1 },
    { "\x1b[%d;%dH", escape_cup },
    { "\x1b[%dA", escape_cuu },
    { "\x1b[A", escape_cuu1 },
    { "\x1b[J", escape_ed },
    { "\x1b[K", escape_el },
    { "\x1b[1K", escape_el1 },
    { "\x1b(B\x1b)0", escape_enacs },
    { "\x1b[H", escape_home },
    // ht
    { "\x1bH", escape_hts },
    // ind
    { "\x1bOq", escape_ka1 },
    { "\x1bOs", escape_ka3 },
    { "\x1bOr", escape_kb2 },
    // kbs
    { "\x1bOp", escape_kc1 },
    { "\x1bOn", escape_kc3 },
    { "\x1bOD", escape_kcub1 },
    { "\x1bOB", escape_kcud1 },
    { "\x1bOC", escape_kcuf1 },
    { "\x1bOA", escape_kcuu1 },
    { "\x1bOM", escape_kent },
    { "\x1bOy", escape_kf0 },
    { "\x1bOP", escape_kf1 },
    { "\x1bOx", escape_kf10 },
    { "\x1bOQ", escape_kf2 },
    { "\x1bOR", escape_kf3 },
    { "\x1bOS", escape_kf4 },
    { "\x1bOt", escape_kf5 },
    { "\x1bOu", escape_kf6 },
    { "\x1bOv", escape_kf7 },
    { "\x1bOl", escape_kf8 },
    { "\x1bOw", escape_kf9 },
    // lf1
    // lf2
    // lf3
    // lf4
    { "\x1b[0i", escape_mc0 },
    { "\x1b[4i", escape_mc4 },
    { "\x1b[5i", escape_mc5 },
    { "\x1b""8", escape_rc },
    { "\x1b[7m", escape_rev },
    { "\x1bM", escape_ri },
    // rmacs
    { "\x1b[?7l", escape_rmam },
    { "\x1b[?1l\x1b>", escape_rmkx },
    { "\x1b[m", escape_rmso },
    { "\x1b[m", escape_rmul },
    // rs2
    { "\x1b""7", escape_sc },
    // sgr
    { "\x1b[0m", escape_sgr0 },
    // smacs
    { "\x1b[?7h", escape_smam },
    { "\x1b[?1h\x1b", escape_smkx },
    { "\x1b""7m", escape_smso },
    { "\x1b[4m", escape_smul },
    { "\x1b[3g", escape_tbc },
};

struct static_escape_code escape_xterm_color [] = {
    { "\x1b[%d;%dH", escape_cup },
};

struct static_escape_code escape_ansi [] = {
    { "\x1b[%d;%dH", escape_cup },
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
    int num_escapes;
    struct static_escape_code *table;

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

    switch( term->type ) {
        case TERM_TYPE_VT100:
            table = escape_vt100;
            num_escapes = sizeof(escape_vt100)/sizeof(struct static_escape_code);
            break;
        case TERM_TYPE_XTERM_COLOR:
            table = escape_xterm_color;
            num_escapes = sizeof(escape_xterm_color)/sizeof(struct static_escape_code);
            break;
        case TERM_TYPE_ANSI:
            table = escape_ansi;
            num_escapes = sizeof(escape_ansi)/sizeof(struct static_escape_code);
            break;
        default:
            num_escapes = 0;
    }
    
    // See if this is a prefix, or is equal to any static escape_code
    for( i = 0; i < num_escapes; i ++ ) {
        int codelen;
        int ret = escape_compare( table[ i ].code, term->escape_code, term->escape_bytes, &codelen );
        if( ret == 2 ) {
            table[ i ].apply_escape_code( term );
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
