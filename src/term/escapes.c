#include <stdio.h>

const char *term_clear_screen = "\x1b[2J";
const char *term_clear_right = "\x1b[K";

const char *term_home_cursor = "\x1b[H";
const char *term_hide_cursor = "\x1b[?25l";
const char *term_show_cursor = "\x1b[?25h";
const char *term_line_cursor = "\x1b[\x36 q";
const char *term_block_cursor = "\x1b[\x32 q";

const char *term_save_buffer = "\x1b[?1049h";
const char *term_restore_buffer = "\x1b[?1049l";

const char *term_underline = "\x1b[4m";
const char *term_bold = "\x1b[1m";
const char *term_no_attr = "\x1b[m";

char *term_set_pos(size_t col, size_t row) {
  static char buf[16];
  // TODO: assert buffer does not overflow
  snprintf(buf, 16, "\x1b[%zu;%zuH", row + 1, col + 1);
  return buf;
}

char *term_set_scroll_region(size_t top, size_t bottom) {
  static char buf[16];
  // TODO: assert buffer does not overflow
  snprintf(buf, 16, "\x1b[%zu;%zur", top +1, bottom + 1);
  return buf;
}

char *term_scroll(ssize_t amount) {
}
