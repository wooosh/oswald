// TODO: replace stddef with sys/types everywhere?
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#include <draw/draw.h>

#include <x.h>
#include <adt/vec.h>
#include <buffer/buffer.h>
#include <event.h>
#include <main.h>
#include <term/escapes.h>

struct DrawState {
  vec_char out;

  // scrolling offsets
  size_t x_offset;
  size_t y_offset;
};

static void draw_at(vec_char *out, size_t x, size_t y) {
  vec_append_str(out, term_set_pos(x, y));
}

// TODO: make this look like buffer dividers
static void draw_status(vec_char *out) {
  size_t x = 0;

  draw_at(out, 0, 0);
  vec_printf(out, "%s%s Oswald Test Build%s%s ", term_underline, term_bold,
             term_no_attr, term_underline);

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
  vec_fill(out, ' ', out->len, E.screen_width - x);
  vec_append_str(out, term_no_attr);
}

// TODO: explain buffer vs render vs screen y 
static size_t draw_line_to_render_y(struct DrawState *ds, struct Buffer *buffer,
                             size_t idx) {
  size_t lines = 0;
  for (struct Buffer *iter = E.buffers.start; iter != buffer;
       iter = iter->next) {
    // +1 to account for buffer title
    lines += iter->lines.len + 1;
    xassert(iter->next, "Buffer was not in buffer list");
  }
  
  lines += idx;
  
  return lines;
}

// TODO: explain viewport vs screen
// delta is the amount before (negative) or after (positive) the row is from the viewport, zero when in viewport
static ssize_t draw_viewport_delta(struct DrawState *ds, size_t render_y) {
  // +1's here are to account for the status bar
  if (render_y < ds->y_offset) {
    return render_y - (ds->y_offset);
  } else if (render_y >= ds->y_offset + E.screen_height) {
    return render_y - ds->y_offset + E.screen_height;
  } else {
    return 0;
  }
}

static void draw_line(struct DrawState *ds, struct Line line, size_t screen_y) {
  draw_at(&ds->out, 0, screen_y);
  vec_append_vec(&ds->out, &line.contents);
}

static void draw_lines(struct DrawState *ds, struct Buffer *buffer, size_t start, size_t len) {
  size_t render_y = draw_line_to_render_y(ds, buffer, start);
  ssize_t delta = draw_viewport_delta(ds, render_y);
  
  if (delta < 0) {
    if (start - delta > len) return;
    start -= delta;
    render_y -= delta;
    len += delta;
  } else if (delta > 0) return;

  for (size_t i=0; i<len; i++) {
    // + 1 to account for status bar
    draw_line(ds, buffer->lines.data[start+i], render_y + i + 1);
  }
}

static void draw_cursor(struct DrawState *ds) {
  // TODO: draw cursor
  if (E.cursor.buffer != NULL) {
  }
}

static void draw_buffer_divider(struct DrawState *ds, struct Buffer *buffer) {
  // get the screen position of the first line and subtract one
  size_t render_y = draw_line_to_render_y(ds, buffer, 0) - 1;
  ssize_t delta = draw_viewport_delta(ds, render_y);

  if (delta < 0) {
    render_y -= delta;
  } else if (delta > 0) return;
    
  draw_at(ds, 0, render_y+1);
  // TODO|CLEANUP: move escape code into escapes.h
  vec_printf(&ds->out, "\x1b[32m-[%.*s]", buffer->title.len, buffer->title.data);
  // 3 accounts for first dash and the two brackets
  for (size_t i=0; i<E.screen_width - 3 - buffer->title.len; i++) {
    vec_append_str(&ds->out, "─");
  }
  vec_append_str(&ds->out, term_no_attr);
}

void draw_event(struct Event e) {
  static struct DrawState ds;

  // draw_scroll_show_cursor(&ds);

  // prefill buffer with hide cursor command
  if (ds.out.len == 0) {
    vec_append_str(&ds.out, term_hide_cursor);
  }

  // TODO: buffer output
  if (e.type == event_open) {
    draw_status(&ds.out);
    draw_buffer_divider(&ds, e.open);
    draw_lines(&ds, e.open, 0, e.open->lines.len);
  }

  if (ds.out.len > 0) {
    draw_cursor(&ds);
    write(STDOUT_FILENO, ds.out.data, ds.out.len);
    vec_truncate(&ds.out, strlen(term_hide_cursor));
  }
}
