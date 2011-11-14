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

struct static_escape_code {
	char *code;
	void (*apply_escape_code)(term_t_i *term);
} static_escape_codes [] = {
	{ "\x1b[K", escape_EL0 }
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

	if( isprefix ) {
		// We ate everything
		return length;
	} else {
		// It's a false escape code - consume a byte so that we won't try to
		// process this again, and discard whatever we have accumulated
		term->escape_bytes = 0;
		term->escape_mode = 0;
		return 1;
	}
}
