#ifndef __LIBTERM_INTERNAL_H__
#define __LIBTERM_INTERNAL_H__

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <libterm.h>

typedef struct {
    // Width in characters
    int width;
    // Height in characters
    int height;
    // Total height of buffer including offscreen
    int history;
    // Grid of characters
    wchar_t **grid;
    // Grid of attributes
    uint32_t **attribs;
    // Color of characters
    uint32_t **colours;
} term_grid;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    bool exists;
} term_dirty_rect;

typedef struct {
    int old_ccol;
    int old_crow;
    bool exists;
} term_dirty_cursor;

typedef struct term_t_i {
    // Current top row of the buffer
    int row;
    // Cursor row
    int crow;
    // Cursor column
    int ccol;
    // Saved cursor position
    int csavedrow;
    int csavedcol;
    // The attributes with which to write
    int cattr;
    // The colour with which to write
    int ccolour;
    // Flag to indicate that memory has been allocated for the grid
    bool allocated;
    // Whether a shell or a special application is being forked
    bool loginShell;
    // Grid of characters and attributes
    term_grid grid;
    // Dirty region of the grid
    term_dirty_rect dirty;
    // Whether the cursor has moved
    term_dirty_cursor dirty_cursor;
    // pid of the child
    pid_t child;
    // pty file descriptor
    int fd;
    // render callback
    void (*update)(term_t handle, int x, int y, int width, int height);
    // cursor callback
    void (*cursor_update)(term_t handle, int old_x, int old_y, int new_x, int new_y);
    // bell callback
    void (*bell)(term_t handle);
    // fork callback
    int (*fork)(term_t handle, int argc, char **argv);
    // Bytes that have been received so far but not send to the terminal
    char *output_bytes;
    int output_byte_count;
    int output_max_bytes;
    // Name of the shell to use
    char *shell;
    // Type of terminal to emulate
    term_type type;
    // Buffer in which to convert a line being retrieved
    char *conversion_buffer;
    // Size of the conversion buffer
    int conversion_buffer_size;
    // Whether to autoexpand
    bool autoexpand;
    int extrawidth;
    // opaque user data
    void *user_data;
} term_t_i;

bool term_allocate_grid(term_grid *grid);
int term_resize_internal( term_t handle, int new_width, int new_height, int new_scrollback, int new_extra_width, term_grid *g );
void term_copy_grid(term_grid *dst, term_grid *src, int offset_y_src, int offset_y_dst, int width, int height);
void term_release_grid(term_grid *grid);
void term_process_output_data(term_t_i *term, char *buf, int length);
int term_send_escape(term_t_i *term, char *buf, int length);
bool term_fork(term_t_i *term);
void term_slay(term_t_i *term);
void term_shiftrows_up(term_t_i *term);
void term_shiftrows_down(term_t_i *term);
void term_update(term_t_i *term);
void term_cursor_update(term_t_i *term);
void term_add_dirty_rect(term_t_i *term, int x, int y, int width, int height);
char *term_find_escape(term_t_i *term, void (*function)(term_t_i *term));

#define TO_S(x) ((term_t_i *)x)
#define TO_H(x) ((term_t)x)

#endif /* __LIBTERM_INTERNAL_H__ */
