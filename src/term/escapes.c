#include <stdio.h>

const char *term_clear_right = "\x1b[K";

const char *term_home_cursor = "\x1b[H"; 
const char *term_hide_cursor = "\x1b[?25l";
const char *term_show_cursor = "\x1b[?25h";
const char *term_line_cursor = "\x1b[\x36 q";
const char *term_block_cursor = "\x1b[\x32 q";

const char *term_save_buffer = "\x1b[?1049h";
const char *term_restore_buffer = "\x1b[?1049l";

char *term_set_pos(size_t row, size_t col) {
  static char buf[16];
  snprintf(buf, 16, "\x1b[%zu;%zuH", row + 1, col + 1); 
  return buf;
}
