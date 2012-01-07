#ifndef __LIBTERM_H__
#define __LIBTERM_H__

#include <stdbool.h>
#include <stdint.h>

typedef void *term_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TERM_TYPE_VT100,
    TERM_TYPE_XTERM_COLOR,
    TERM_TYPE_ANSI,
    TERM_TYPE_MAX
} term_type;

typedef enum {
    TERM_KEY_UP,
    TERM_KEY_DOWN,
    TERM_KEY_LEFT,
    TERM_KEY_RIGHT
} term_special_key;

// Attributes
#define TERM_ATTRIB_BOLD        (1<<0)
#define TERM_ATTRIB_UNDERSCORE  (1<<1)
#define TERM_ATTRIB_BLINK       (1<<2)
#define TERM_ATTRIB_REVERSE     (1<<3)
#define TERM_ATTRIB_CONCEALED   (1<<4)
#define TERM_ATTRIB_STANDOUT    (1<<5)

#define TERM_COLOR_BLACK        1
#define TERM_COLOR_RED          2
#define TERM_COLOR_GREEN        3
#define TERM_COLOR_YELLOW       4
#define TERM_COLOR_BLUE         5
#define TERM_COLOR_MAGENTA      6
#define TERM_COLOR_CYAN         7
#define TERM_COLOR_WHITE        8

#define TERM_FG_SHIFT           0
#define TERM_FG_MASK            0xFF
#define TERM_BG_SHIFT           8
#define TERM_BG_MASK            0xFF00


// Create a terminal object with the specified dimensions. Scrollback
// indicates the number of lines to be preserved after they have scrolled
// off the display
bool term_create(term_t *handle);

// Specify what type of terminal to emulate
int term_set_emulation(term_t handle, term_type type);

// Set a shell to use after forking
bool term_set_shell(term_t handle, char *shell);
// Set the program to use instead of a login shell
bool term_set_program(term_t handle, char *program);
// Set an arbitary callback to use after forking. The signature is the same as
// main. It's a good idea to call term_set_program or term_set_shell before
// this to specify arguments to pass
bool term_set_fork_callback(term_t handle, int (*callback)(term_t handle, int argc, char **argv));

// Begin a terminal session. Set up all parameters before calling this function
bool term_begin( term_t handle, int width, int height, int scrollback );

// Functions to set and get an opaque blob of data attached to the terminal instance
void term_set_user_data(term_t handle, void *user_data);
void *term_get_user_data(term_t handle);

// Scroll to a row
void term_scroll( term_t handle, int row );

// Resize the terminal
int term_resize( term_t handle, int new_width, int new_height, int new_scrollback );

// Callback functions for when the screen state changes
void term_register_update(term_t handle, void (*update)(term_t handle, int x, int y, int width, int height));

// Callback functions for when the cursor moves
void term_register_cursor(term_t handle, void (*update)(term_t handle, int old_x, int old_y, int new_x, int new_y));

// Callback functions for when the cursor moves
void term_register_bell(term_t handle, void (*bell)(term_t handle));

// Get a file descriptor to wait for data from the child process. Waiting on
// this file handle should be integrated in the programs event processing loop
int term_get_file_descriptor(term_t handle);

// Retrieve the grid of characters
const wchar_t **term_get_grid(term_t handle);
const uint32_t **term_get_attribs(term_t handle);
const uint32_t **term_get_colours(term_t handle);
// Get the dimensions of the grid
int term_get_grid_size(term_t handle, int *w, int *h);
// Get the cursor position on the grid
int term_get_cursor_pos(term_t handle, int *x, int *y);
// Retrieve a UTF-8 version of a particular row
const char *term_get_line(term_t handle, int row);

// Retrieve grid propeties
int term_get_width(term_t handle);
int term_get_height(term_t handle);

// Convert an attribute and colour pair into RGB888
uint32_t term_get_fg_color(uint32_t attrib, uint32_t colour);
uint32_t term_get_bg_color(uint32_t attrib, uint32_t colour);

// This function should be called to process data from the child process
bool term_process_child(term_t handle);

// Send input to the shell
void term_send_data(term_t handle, const char *string, int length);
void term_send_special(term_t handle, term_special_key key);

// Release all resources associated with a terminal
void term_free(term_t handle);

#ifdef __cplusplus
}
#endif

#endif /* __LIBTERM_H__ */
