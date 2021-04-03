// TODO: replace stddef with sys/types everywhere?
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <draw/draw.h>

#include <adt/vec.h>
#include <buffer/buffer.h>
#include <event.h>
#include <main.h>
#include <term/escapes.h>
#include <x.h>

struct DrawState {
  vec_char out;

  // scrolling offsets
  size_t x_offset;
  // TODO: make render_y
  size_t y_offset;
};

// common suffixes/abbrievations:
// by - buffer y
// ry - render y
// sy - screen y

// a buffer coordinate points to a line in a buffer, and the referenced line
// can be found via buffer.lines.data[y]
typedef size_t BufferY;
// a render coordinate represents a position the entire scrolling area
typedef size_t RenderY;
// a screen coordinate represents an addressable cell in the drawing area
// (typically a terminal)
typedef size_t ScreenY;

static RenderY buffer_y_to_render_y(struct Buffer *buffer, BufferY idx) {
  RenderY lines = 0;
  for (struct Buffer *iter = E.buffers.start; iter != buffer;
       iter = iter->next) {
    // +1 to account for buffer title
    lines += iter->lines.len + 1;
    xassert(iter->next, "Buffer was not in buffer list");
  }

  lines += idx;

  return lines;
}

// TODO: coords.h

// TODO: assert render y is on screen
static ScreenY render_y_to_screen_y(struct DrawState *ds, RenderY ry) {
  return ry - ds->y_offset + 1;
}

// TODO: do we have an off by one error here because of status bar
// clamps a render range to be within the screen edges
static void render_range_clamp_screen(struct DrawState *ds, RenderY *start,
                                      size_t *len) {
  // Check if range is on the screen at all
  if (*start + *len < ds->y_offset + 1 ||
      *start > ds->y_offset + E.screen_height - 1) {
    *len = 0;
    return;
  }

  if (*start < ds->y_offset + 1) {
    *len -= ds->y_offset - *start;
    *start = ds->y_offset;
  }

  if (*start + *len > ds->y_offset + (E.screen_height - 1)) {
    *len = ds->y_offset + (E.screen_height - 1) - *start;
  }
}
static void buffer_range_clamp_screen(struct DrawState *ds,
                                      struct Buffer *buffer, BufferY *start_by,
                                      size_t *len) {
  RenderY orig_start_ry = buffer_y_to_render_y(buffer, *start_by);
  RenderY clamp_start_ry = orig_start_ry;
  render_range_clamp_screen(ds, &clamp_start_ry, len);
  *start_by -= orig_start_ry - clamp_start_ry;
}

static void draw_at(vec_char *out, size_t x, ScreenY y) {
  vec_append_str(out, term_set_pos(x, y));
}

static void draw_line(struct DrawState *ds, ScreenY y, struct Line line) {
  draw_at(&ds->out, 0, y);
  vec_append_str(&ds->out, term_clear_right);
  vec_append_vec(&ds->out, &line.contents);
}

static void draw_buffer_lines(struct DrawState *ds, struct Buffer *buffer,
                              BufferY start_by, size_t len) {
  buffer_range_clamp_screen(ds, buffer, &start_by, &len);
  RenderY start_ry = buffer_y_to_render_y(buffer, start_by);
  ScreenY start_sy = render_y_to_screen_y(ds, start_ry);

  for (size_t i = 0; i < len; i++) {
    draw_line(ds, start_sy + i, buffer->lines.data[start_by + i]);
  }
}

static void draw_buffer_divider(struct DrawState *ds, struct Buffer *buffer) {
  // get the render y of the line before the first line
  RenderY divider_ry = buffer_y_to_render_y(buffer, 0) - 1;

  size_t len = 1;
  render_range_clamp_screen(ds, &divider_ry, &len);

  if (len == 1) {
    ScreenY divider_sy = render_y_to_screen_y(ds, divider_ry);
    draw_at(ds, 0, divider_sy);

    // TODO|CLEANUP: move escape code into escapes.h
    vec_printf(&ds->out, "\x1b[32m-[%.*s]", buffer->title.len,
               buffer->title.data);
    // 3 accounts for first dash and the two brackets
    for (size_t i = 0; i < E.screen_width - 3 - buffer->title.len; i++) {
      vec_append_str(&ds->out, "─");
    }
    vec_append_str(&ds->out, term_no_attr);
  }
}

static void draw_screen_range(struct DrawState *ds, ScreenY start_sy,
                              size_t len) {
  // RenderY start_ry = render_y_to_screen_y(ds, start_sy);
  // RenderY end_ry = render_y_to_screen_y(ds, start_sy + len);
  // RenderY lines = 0;
  for (struct Buffer *iter = E.buffers.start; iter != NULL; iter = iter->next) {
    // TODO: non naive impl that doesn't redraw whole screen
    // insert buffer divider
    draw_buffer_divider(ds, iter);
    draw_buffer_lines(ds, iter, 0, iter->lines.len);
  }
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

// TODO: scroll.c
static void draw_scroll_to_show_cursor(struct DrawState *ds) {
  if (E.cursor.buffer != NULL) {
    RenderY cursor_ry = buffer_y_to_render_y(E.cursor.buffer, E.cursor.y);

    // +1 to account for statusbar
    if (cursor_ry >= ds->y_offset + E.screen_height - 1) {
      // +2 because >= and statusbar is 1 tall
      size_t delta = cursor_ry - (ds->y_offset + E.screen_height) + 2;
      ds->y_offset += delta;
      // TODO:
      draw_screen_range(ds, ds->y_offset + E.screen_height - delta, delta);
    } else if (cursor_ry < ds->y_offset) {
      size_t delta = (ds->y_offset + 1) - cursor_ry;
      ds->y_offset = cursor_ry;
      draw_screen_range(ds, ds->y_offset, delta);
    }
  }
  /*
    // TODO: move into function
    vec_append_str(&ds->out, term_set_scroll_region(1, E.screen_height));
    ds->y_offset += delta;

    // TODO: move escape code to term/escapes.h
    // TODO: check if we need to redraw the whole screen anyway
    if (delta < 0) {
      // TODO: vec_fill_str that reserves space first
      for (size_t i=0; i < -delta; i++) {
        draw_at(&ds->out, 0, 1);
        vec_append_str(&ds->out, "\x1bM");
      }
      draw_screen_lines(ds, ds->y_offset - delta, -delta);
    } else if (delta > 0) {
      draw_at(&ds->out, 0, E.screen_height);
      vec_fill(&ds->out, '\n', ds->out.len, delta);
      vec_append_str(&ds->out, term_clear_screen);
      draw_screen_lines(ds, ds->y_offset + E.screen_height - delta, delta);
  //    for (int i=0; i<delta; i++) {
  //      draw_at(&ds->out, 0, ds->y_offset + E.screen_height - delta + i);
  //      vec_append_str(&ds->out, "mmmm");
  //    }
    }
  */
}

static void draw_cursor(struct DrawState *ds) {
  if (E.cursor.buffer != NULL) {
    // assume the cursor is in the viewport
    RenderY ry = buffer_y_to_render_y(E.cursor.buffer, E.cursor.y);
    ScreenY sy = render_y_to_screen_y(ds, ry);
    draw_at(&ds->out, E.cursor.x, sy);
    vec_append_str(&ds->out, term_show_cursor);
  }
}

// TODO: memoize?
void draw_event(struct Event e) {
  static struct DrawState ds;
  // prefill buffer with hide cursor command
  if (ds.out.len == 0) {
    vec_append_str(&ds.out, term_hide_cursor);
  }

  // TODO: add way to tell renderer to queue events during batch operations?
  // then dispatch redraw event?

  draw_scroll_to_show_cursor(&ds);

  switch (e.type) {
  case event_open:
    draw_status(&ds.out);
    draw_buffer_divider(&ds, e.open);
    draw_buffer_lines(&ds, e.open, 0, e.open->lines.len);
    break;
  case event_edit:
    if (e.edit->type == EditChanged) {
      draw_buffer_lines(&ds, e.edit->buffer, e.edit->start, e.edit->len);
    } else {
      // TODO: smart redraw using scrolling
    }
    break;
  }

  if (e.type == event_mark_move || ds.out.len > 0) {
    draw_cursor(&ds);
    write(STDOUT_FILENO, ds.out.data, ds.out.len);
    vec_truncate(&ds.out, strlen(term_hide_cursor));
  }
}
