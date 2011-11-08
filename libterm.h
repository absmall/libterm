#include <stdbool.h>
typedef void *term_t;

// Create a terminal object with the specified dimensions. Scrollback
// indicates the number of lines to be preserved after they have scrolled
// off the display
bool term_create(int width, int height, int scrollback, term_t *handle);

// Release all resources associated with a terminal
void term_free(term_t handle);
