#include <stdlib.h>
#include <libterm_internal.h>

void term_process_output_data(term_t_i *term, char *buf, int length)
{
	int i;

	for( i = 0; i < length; i ++ ) {
		switch(buf[i]) {
			case '\n':
				term->crow++;
				term->ccol=0;
				if( term->crow >= term->height ) {
					term_shiftrows(term);
				}
				break;
			default:
				term->grid[term->crow][term->ccol++] = buf[i];
		}
	}
	
	if( term->update != NULL ) term->update(TO_H(term), 0, 0, term->width, term->height);
}
