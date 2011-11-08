#include <stdint.h>
#include "libterm.h"

typedef struct term_t_i {
	int width;
	int height;
	int history;
	uint32_t **grid;
	uint32_t **attribs;
} term_t_i;

#define TO_S(x) ((term_t_i *)x)
#define TO_H(x) ((term_t)x)
