#include "buffer/mark.h"
#include "buffer/buffer.h"
#include "main.h"
#include "highlight.h"

#include <stddef.h>

#include <meraki/output.h>

const struct MerakiStyle selection_style = {{Meraki8Color, -1}, {Meraki8Color, -1}, MerakiReverse};

void highlight_cursor(struct Buffer *buffer, size_t line, vec_style *highlight) {
  if (E.cursor.buffer != buffer) return;

  struct Mark start = E.cursor;
  struct Mark end = E.anchor;
  if (!mark_cmp(&start, &end)) {
    start = E.anchor;
    end = E.cursor;
  }

  if (start.y == end.y && line == start.y) {
    vec_set(highlight, selection_style, start.x, end.x - start.x + 1);
  } else if (line > start.y && line < end.y) {
    vec_set(highlight, selection_style, 0, highlight->len);
  } else if (line == start.y) {
    vec_set(highlight, selection_style, start.x, highlight->len - start.x);
  } else if (line == end.y) {
    vec_set(highlight, selection_style, 0, end.x);
  }
}
