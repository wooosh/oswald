#include <meraki/term.h>
#include <meraki/output.h>

#include "buffer/buffer.h"
#include "event.h"
#include "main.h"
#include "adt/vec.h"

struct DrawState {
  struct MerakiOutput *out;
  // terminal size
  size_t width;
  size_t height;

  // used for scrolling
  size_t offset_x;
  size_t offset_y;

  // buffer for currently drawn line
  vec_char line;
  vec_style style;
};

// writes the current line to the screen then clears it
static void draw_line(struct DrawState *ds, size_t y) {
  meraki_output_draw(ds->out, y, ds->line.len, ds->line.data, ds->style.data);
  vec_truncate(&ds->line, 0);
  vec_truncate(&ds->style, 0);
}

// TODO: document how this function works
static void style_previous(struct DrawState *ds, struct MerakiStyle style) {
  // TODO: document which vec function fields cannot contain expressions or
  // vector fields in vec.h
  vec_fill(&ds->style, style, ds->style.len, ds->line.len - ds->style.len); 
}

static void draw_status(struct DrawState *ds) {
  vec_append_str(&ds->line, " Oswald Test Build ");

  // TODO: pull styles from config
  struct MerakiStyle s = { {0, {-1}}, {0, {-1}}, MerakiBright};
  style_previous(ds, s);

  draw_line(ds, 0);
}

static void draw_buffer_title(struct DrawState *ds, struct Buffer *buffer, size_t y) {
  vec_printf(&ds->line, " [%.*s]", buffer->title.len, buffer->title.data);
  vec_fill(&ds->line, ' ', ds->line.len, ds->width - ds->line.len);
 
  struct MerakiStyle s = {{0, {-1}}, {0, {-1}}, MerakiReverse};
  style_previous(ds, s);

  draw_line(ds, y);
}

static void draw_screen(struct DrawState *ds) {
  size_t lines_read = 0;
  // start at one since the status bar is at the top 
  size_t screen_y = 1;
  for (size_t i=0; i<E.buffers.len; i++) {
    struct Buffer *buffer = E.buffers.data[i];

    // draw buffer title
    lines_read++;
    if (lines_read >= ds->offset_y + ds->height) break;
    if (lines_read >= ds->offset_y) {
      draw_buffer_title(ds, buffer, screen_y);
      screen_y++;
    }

    // draw buffer contents
    if (lines_read + buffer->lines.len > ds->offset_y) {
      size_t buffer_y = 0;

      if (lines_read < ds->offset_y) {
        buffer_y = ds->offset_y - lines_read;
      }

      for (; buffer_y < buffer->lines.len; buffer_y++) {
        struct Line l = buffer->lines.data[buffer_y];
        meraki_output_draw(ds->out, screen_y, l.contents.len, l.contents.data, l.highlight.data);
        screen_y++;
        if (screen_y >= ds->height) break;
      }
      
      if (screen_y >= ds->height) break;
    }
  }
}

void draw_event(struct Event e) {
  static struct DrawState ds = {NULL};

  if (ds.out == NULL) {
    ds.out = meraki_term_output(E.term);
    // TODO: use real height/width
    meraki_term_size(E.term, &ds.width, &ds.height);
    vec_init(&ds.line);
    vec_init(&ds.style);
  }

  draw_status(&ds);
  draw_screen(&ds);

  meraki_output_commit(ds.out);
}
