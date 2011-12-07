#ifndef __LIBTERM_INTERNAL_H__
#define __LIBTERM_INTERNAL_H__

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
    // Grid of characters and attributes
    term_grid grid;
    // pid of the child
    pid_t child;
    // pty file descriptor
    int fd;
    // render callback
    void (*update)(term_t handle, int x, int y, int width, int height);
    // cursor callback
    void (*cursor_update)(term_t handle, int x, int y);
    // bell callback
    void (*bell)(term_t handle);
    // Whether we're in the midst of processing an escape code
    bool escape_mode;
    // Bytes that have been received so far for an escape code
    char *escape_code;
    int escape_bytes;
    int escape_max_bytes;
    // Name of the shell to use
    char *shell;
    // Type of terminal to emulate
    term_type type;
    // Buffer in which to convert a line being retrieved
    char *conversion_buffer;
    // Size of the conversion buffer
    int conversion_buffer_size;
    // opaque user data
    void *user_data;
} term_t_i;

bool term_allocate_grid(term_grid *grid);
void term_copy_grid(term_grid *dst, term_grid *src);
void term_release_grid(term_grid *grid);
void term_process_output_data(term_t_i *term, char *buf, int length);
int term_send_escape(term_t_i *term, char *buf, int length);
bool term_fork(term_t_i *term);
void term_slay(term_t_i *term);
void term_shiftrows(term_t_i *term);

#define TO_S(x) ((term_t_i *)x)
#define TO_H(x) ((term_t)x)

#endif /* __LIBTERM_INTERNAL_H__ */
