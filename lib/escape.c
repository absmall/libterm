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

    for( i = 0; i < term->output_byte_count; i ++ ) {
        if( i == 0 ) {
            if( term->output_bytes[ i ] != 27 ) return 0;
        } else if( i == 1 ) {
            if( term->output_bytes[ i ] != '[' ) {
                return 0;
            } else {
                state ++;
            }
        } else if( state == 1 ) {
            if( term->output_bytes[ i ] == 'm'
             && isdigit( term->output_bytes[ i - 1 ] ) ) {
                *length = i + 1;
                return 2;
            }
            else if( !isdigit( term->output_bytes[ i ] )
             && term->output_bytes[ i ] != ';' ) return 0;
        }
    }

    return 1;
}

struct static_escape_code {
    char *code;
    void (*apply_escape_code)(term_t_i *term);
};

struct static_escape_code escape_vt100 [] = {
    { "\x7", escape_bel },
    { "\x1b""5m", escape_blink },
    { "\x1b""1m", escape_bold },
    { "\x1b[2J", escape_clear },
    { "\xd", escape_cr },
    { "\x1b[%d;%dr", escape_csr },
    { "\x1b[%dD", escape_cub },
    { "\x8", escape_cub1 },
    { "\x1b[%dB", escape_cud },
    { "\xa", escape_cud1 },
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
    { "\x9", escape_ht },
    { "\x1bH", escape_hts },
    { "\xa", escape_ind },
    { "\x1bOq", escape_ka1 },
    { "\x1bOs", escape_ka3 },
    { "\x1bOr", escape_kb2 },
    { "\x8", escape_kbs },
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
    { "\xf", escape_rmacs },
    { "\x1b[?7l", escape_rmam },
    { "\x1b[?1l\x1b>", escape_rmkx },
    { "\x1b[m", escape_rmso },
    { "\x1b[m", escape_rmul },
    // rs2
    { "\x1b""7", escape_sc },
    // sgr
    { "\x1b[0m", escape_sgr0 },
    { "\xe", escape_smacs },
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
    { "\x7", escape_bel },
    { "\x1b[5m", escape_blink },
    { "\x1b[1m", escape_bold },
    //{ "\x1b[Z", escape_cbt },
    { "\x1b[H\x1b[J", escape_clear },
    { "\xd", escape_cr },
    { "\x1b%dD", escape_cub },
    { "\x1b[D", escape_cub1 },
    { "\x1b[%dB", escape_cud },
    { "\x1b[B", escape_cud1 },
    { "\x1b[%dC", escape_cuf },
    { "\x1b[C", escape_cuf1 },
    { "\x1b[%d;%dH", escape_cup },
    { "\x1b%dA", escape_cuu },
    { "\x1b[A", escape_cuu1 },
    //{ "\x1b%dP", escape_dch },
    //{ "\x1b[P", escape_dch1 },
    //{ "\x1b%dM", escape_dl },
    //{ "\x1b[M", escape_dl1 },
    //{ "\x1b%dX", escape_ech },
    { "\x1b[J", escape_ed },
    { "\x1b[1K", escape_el1 },
    { "\x1b[H", escape_home },
    //{ "\x1b%dG", escape_hpa },
    { "\x1b[I", escape_ht },
    { "\x1bH", escape_hts },
    //{ "\x1b%d@", escape_ich },
    //{ "\x1b[%dL", escape_il },
    //{ "\x1b[L", escape_il1 },
    { "\xa", escape_ind },
    //{ "\x1b[%dS", escape_indn },
    { "\x1b[8m", escape_invis },
    { "\x8", escape_kbs },
    //{ "\x1b[Z", escape_kcbt },
    { "\x1b[D", escape_kcub1 },
    { "\x1b[B", escape_kcud1 },
    { "\x1b[C", escape_kcuf1 },
    { "\x1b[A", escape_kcuu1 },
    //{ "\x1b[H", escape_khome },
    //{ "\x1b[L", escape_kich1 },
    { "\x1b[4i", escape_mc4 },
    { "\x1b[5i", escape_mc5 },
    { "\r\x1b[S", escape_nel },
    //{ "\x1b[39;49m", escape_op },
    //{ "%c\x1b%d-%db", escape_rep },
    { "\x1b[7m", escape_rev },
    //{ "\x1b[%dT", escape_rin },
    { "\x1b[10m", escape_rmacs },
    //{ "\x1b[10m", escape_rmpch },
    { "\x1b[m", escape_rmso },
    { "\x1b[m", escape_rmul },
    //{ "\x1b(B", escape_s0ds },
    //{ "\x1b)B", escape_s1ds },
    //{ "\x1b*B", escape_s2ds },
    //{ "\x1b+B", escape_s3ds },
    //{ "\x1b""4%dm", escape_setab },
    //{ "\x1b""3%dm", escape_setaf },
    //{ "", escape_sgr },
    { "\x1b[0;10m", escape_sgr0 },
    { "\x1b[11m", escape_smacs },
    //{ "\x1b[11m", escape_smpch },
    { "\x1b[7m", escape_smso },
    { "\x1b[4m", escape_smul },
    { "\x1b[3g", escape_tbc },
    //{ "\x1b[%d;%dR", escape_u6 },
    //{ "\x1b[6n", escape_u7 },
    //{ "\x1b[?%[;0123456789]c", escape_u8 },
    //{ "\x1b[c", escape_u9 },
    { "\x1b[%dd", escape_vpa },
};

struct dynamic_escape_code {
    int (*code_match)(term_t_i *term, int *length);
    void (*apply_escape_code)(term_t_i *term);
} dynamic_escape_codes[] = {
    { match_sgm, escape_sgm }
};

char *term_find_escape(term_t_i *term, void (*function)(term_t_i *term))
{
    int i;
    int num_escapes;
    struct static_escape_code *table;

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
        if( table[ i ].apply_escape_code == function ) {
            return table[ i ].code;
        }
    }
    
    return NULL;
}

int term_send_escape(term_t_i *term, char *buf, int length)
{
    bool isprefix = false;
    int i;
    int num_escapes;
    struct static_escape_code *table;

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
        int ret = escape_compare( table[ i ].code, buf, length, &codelen );
        if( ret == 2 ) {
            table[ i ].apply_escape_code( term );
            return codelen;
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
            return codelen;
        } else if( ret == 1 ) {
            isprefix = true;
        }
    }

    if( isprefix ) {
        // Still a prefix, keep looking
        return 0;
    } else {
        // Not an escape code
        return -1;
    }
}
