#pragma once

#include <adt/vec.h>

// null terminated escape code constants
extern const char *term_clear_right;

// returns the cursor to 0,0
extern const char *term_home_cursor; 

extern const char *term_hide_cursor;
extern const char *term_show_cursor;

// used to change the cursor shape
extern const char *term_line_cursor;
extern const char *term_block_cursor;

// used to keep the terminal history after we start and exit the editor
extern const char *term_save_buffer;
extern const char *term_restore_buffer;

/*
 * moves the cursor to a given position on the screen
 * the top left cell is 0,0
 * the returned string is valid until you call term_set_pos again
 */
char *term_set_pos(size_t row, size_t col);
