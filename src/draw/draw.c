// TODO: replace stddef with sys/types everywhere?
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include <draw/draw.h>

#include <main.h>
#include <adt/vec.h>
#include <term/escapes.h>
#include <buffer/buffer.h>
#include <event.h>

// TODO: winsize

void draw_at(vec_char *out, size_t x, size_t y) {
  vec_append_str(out, term_set_pos(x, y));
}

void draw_status(vec_char *out) {
  size_t x = 0;
  
  draw_at(out, 0, 0);
  vec_printf(out, "%s%s Oswald Test Build%s%s ", term_underline, term_bold, term_no_attr, term_underline);
 
  x += strlen(" Oswald Test Build ");
 
  if (E.cursor.buffer != NULL) {
    vec_append_vec(out, &E.cursor.buffer->title);
    x += E.cursor.buffer->title.len;
  } else {
    vec_append_str(out, "No Buffer Selected");
    x += strlen("No Buffer Selected");
  }

  // print enough padding for the underline attribute to cover the width of
  // the screen
  vec_fill(out, ' ', out->len, E.screen_width-x);
  vec_append_str(out, term_no_attr);
}

void draw_portion_bar(vec_char *out);
void draw_line(vec_char *out);

void draw_cursor(vec_char *out) {
  // TODO: draw cursor
  if (E.cursor.buffer != NULL) {

  }
}

void draw_event(struct Event e) {
  static vec_char out;

  // prefill buffer with hide cursor command
  if (out.len == 0) {
    vec_append_str(&out, term_hide_cursor);
  }

  // TODO: buffer output
  if (e.type == event_open) {
    draw_status(&out);
    struct Buffer *buffer = e.open;

  }

  if (out.len > 0) {
    draw_cursor(&out);    
    write(STDOUT_FILENO, out.data, out.len);
    vec_truncate(&out, strlen(term_hide_cursor));
  }
}
