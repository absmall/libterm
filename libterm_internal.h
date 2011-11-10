#include <stdint.h>
#include "libterm.h"

typedef struct term_t_i {
	// Width in characters
	int width;
	// Height in characters
	int height;
	// Total height of buffer including offscreen
	int history;
	// Current top row of the buffer
	int row;
	// Cursor row
	int crow;
	// Cursor column
	int ccol;
	// Grid of characters
	uint32_t **grid;
	// Grid of attributes
	uint32_t **attribs;
	// pid of the child
	pid_t child;
	// pty file descriptor
	int fd;
	// render callback
	void (*update)(term_t handle, int x, int y, int width, int height);
} term_t_i;

void term_process_output_data(term_t_i *term, char *buf, int length);
bool term_fork(term_t_i *term);
void term_slay(term_t_i *term);

#define TO_S(x) ((term_t_i *)x)
#define TO_H(x) ((term_t)x)
