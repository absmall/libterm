#include <stdlib.h>
#include <libterm_internal.h>

void term_process_output_data(term_t_i *term, char *buf, int length)
{
	int i;

	for( i = 0; i < length; i ++ ) {
		switch(buf[i]) {
			case '\r':
				break;
			case '\n':
				term->ccol=0;
				term->crow++;
				if( term->crow >= term->height ) {
					term_shiftrows(term);
				}
				break;
			default:
				if( term->crow < term->height && term->ccol < term->width ) {
					term->grid[term->crow][term->ccol] = buf[i];
				}
				term->ccol++;
		}
	}
	
	if( term->update != NULL ) term->update(TO_H(term), 0, 0, term->width, term->height);
}
