#include <stdlib.h>
#include <libterm_internal.h>

void term_process_output_data(term_t_i *term, char *buf, int length)
{
	int i;

	for( i = 0; i < length; i ++ ) {
		switch(buf[i]) {
			case '\n':
				term->ccol++;
				term->crow=0;
				break;
			default:
				term->grid[term->ccol][term->crow++] = buf[i];
		}
	}
	
	if( term->update != NULL ) term->update(TO_H(term), 0, 0, term->width, term->height);
}
