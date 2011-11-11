#include <stdbool.h>
#include <stdint.h>

typedef void *term_t;

// Create a terminal object with the specified dimensions. Scrollback
// indicates the number of lines to be preserved after they have scrolled
// off the display
bool term_create(int width, int height, int scrollback, term_t *handle);

// Callback functions for when the screen state changes
void term_register_update(term_t handle, void (*update)(term_t handle, int x, int y, int width, int height));

// Get a file descriptor to wait for data from the child process. Waiting on
// this file handle should be integrated in the programs event processing loop
int term_get_file_descriptor(term_t handle);

// Retrieve the grid of characters
const uint32_t **term_get_grid(term_t handle);

// This function should be called to process data from the child process
bool term_process_child(term_t handle);

// Send input to the shell
void term_send_data(term_t handle, char *string, int length);

// Release all resources associated with a terminal
void term_free(term_t handle);
