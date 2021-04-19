#include <meraki/term.h>
#include <meraki/output.h>

#include <stdio.h>

#include "highlight.h"
#include "buffer/buffer.h"
#include "event.h"
#include "main.h"
#include "adt/vec.h"
#include "x.h"

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
  xassert(y < ds->height, NULL); 
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
  // TODO: truncate status to screen
  vec_append_str(&ds->line, " Oswald Test Build ");

  // TODO: pull styles from config
  struct MerakiStyle s = { {0, {-1}}, {0, {-1}}, MerakiBright | MerakiUnderscore};
  style_previous(ds, s);

  vec_append_vec(&ds->line, &E.mode);
  vec_fill(&ds->line, ' ', ds->line.len, ds->width - ds->line.len);
  
  s.attr = MerakiUnderscore;
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
    if (lines_read > ds->offset_y) {
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
        vec_append_vec(&ds->line, &buffer->lines.data[buffer_y]);
        // add space for cursor at end of line
        vec_push(&ds->line, ' ');

        struct MerakiStyle s = {{Meraki8Color, -1}, {Meraki8Color, -1}, MerakiNone};
        vec_fill(&ds->style, s, 0, ds->line.len);
        highlight_line(buffer, buffer_y, &ds->style);

        // TODO: unicode support for trimming
        vec_splice(&ds->line, 0, ds->offset_x);
        vec_splice(&ds->style, 0, ds->offset_x);
        vec_truncate(&ds->line, ds->width);
        vec_truncate(&ds->style, ds->width);
        draw_line(ds, screen_y);
        
        screen_y++;
        lines_read++;
        if (screen_y >= ds->height) break;
      }
      
      if (screen_y >= ds->height) break;
    }
  }

  for (;screen_y < ds->height; screen_y++) {
    struct MerakiStyle s = {{Meraki8Color, -1}, {Meraki8Color, -1}, MerakiDim};
    vec_push(&ds->style, s);
    vec_push(&ds->line, '~');
    draw_line(ds, screen_y);
  }
}

static void clamp_cursor(struct DrawState *ds) {
  if (E.cursor.buffer == NULL) return;
  // get viewport y of cursor
  size_t vy = 0;
  for (int i=0; E.buffers.data[i] != E.cursor.buffer; i++) {
    // + 1 for buffer title
    vy += E.buffers.data[i]->lines.len + 1;
  }
  vy += E.cursor.y + 1;

  // TODO: use clamp function
  if (vy < ds->offset_y) {
    ds->offset_y = vy;
  } else if (vy >= (ds->height - 1) + ds->offset_y) {
    // TODO: explain how this math works because ???
    ds->offset_y = vy - (ds->height - 1) + 1;
  }

  // x scrolling
  if (E.cursor.x < ds->offset_x) {
    ds->offset_x = E.cursor.x;
  } else if (E.cursor.x >= ds->offset_x + ds->width - 1) {
    ds->offset_x = E.cursor.x - (ds->width-1);
  }
}

void draw_event(struct Event e) {
  static struct DrawState ds = {NULL};

  // TODO: event switch to add fast paths for expensive operations like
  // inserting lines

  if (ds.out == NULL) {
    ds.out = meraki_term_output(E.term);
    meraki_term_size(E.term, &ds.width, &ds.height);
    vec_init(&ds.line);
    vec_init(&ds.style);
    meraki_output_cursor_hide(ds.out);
  }

  clamp_cursor(&ds);
  draw_status(&ds);
  draw_screen(&ds);

  meraki_output_commit(ds.out);
}
