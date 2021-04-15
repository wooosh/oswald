#include "string.h"

#include <adt/vec.h>
#include <buffer/buffer.h>
#include <buffer/mark.h>
#include <event.h>
#include <x.h>

vec_char *mark_line(struct Mark *m) {
  return &m->buffer->lines.data[m->y];
}

// TODO: should we use size_t everywhere because it messes up signed comparisons
// clamps value to [min, max]
ssize_t clamp(ssize_t min, ssize_t max, ssize_t val) {
  if (val < min)
    return min;
  if (val > max)
    return max;
  return val;
}

// TODO: mark_move that makes left on position zero move up a line, and right on
// position go down a line

// negative = left/up
// positive = right/down
void mark_move_rel(struct Mark *m, ssize_t x, ssize_t y) {
  m->y = clamp(0, m->buffer->lines.len - 1, m->y + y);
  m->x = clamp(0, mark_line(m)->len, m->x + x);
  dispatch_event((struct Event){event_mark_move, .mark_move = m});
}

// TODO: should this be mark_order(mark **start, mark **end) and automatically
//  swap marks to be in order?
// TODO: assert in same buffer
// TODO: assert valid mark (make a function for this
bool mark_cmp(struct Mark *a, struct Mark *b) {
  // marks on different lines
  if (a->y < b->y)
    return true;
  if (a->y > b->y)
    return false;

  // marks on same line
  if (a->x <= b->x)
    return true;
  else
    return false;
}

// TODO: multiline delete (maybe delete the area between two marks?
void mark_delete(struct Mark *a, struct Mark *b) {
  struct Mark *start = a;
  struct Mark *end = b;

  vec_char *line_start = mark_line(start);
  vec_char *line_end = mark_line(end);

  // make sure marks are actually in order
  if (!mark_cmp(start, end)) {
    start = b;
    end = a;
  }

  // delete within single line
  if (start->y == end->y) {
    vec_splice(line_start, start->x, end->x);

    *end = *start;

    struct EditEvent ev = {start->buffer, start->y, 1, EditChanged}; 
    dispatch_event((struct Event){event_edit, .edit = &ev});
    return;
  }

  // multline deletes

  // delete start and end lines first because the positions will not have to be
  // adjusted when we remove the lines between
  vec_truncate(line_start, start->x);

  vec_splice(line_end, 0, end->x);

  vec_line *lines = &start->buffer->lines;
  // remove all of the lines in between
  for (size_t i = start->y + 1; i < end->y; i++) {
    // TODO: line_destroy() function
    vec_destroy(&lines->data[i]);
  }

  vec_splice(lines, start->y + 1, end->y);
}

// TODO: cleanup
void mark_insert(struct Mark *m, vec_const_char str, bool keep_pos) {
  struct EditEvent ev = {m->buffer, m->y, 1, EditChanged};

  char *nl = memchr(str.data, '\n', str.len);
  // TODO: combine these two cases
  if (nl == NULL) {
    vec_insert_vec(mark_line(m), &str, m->x);
    if (!keep_pos) m->x += str.len;
  } else {
    // split current line in two at the marks position, and save the last
    // half for later as the last line
    vec_char last_line;
    vec_init(&last_line);
    vec_append_vec(&last_line, mark_line(m));
    vec_splice(&last_line, 0, m->x);
    vec_splice(mark_line(m), m->x, mark_line(m)->len);
   
    ev.len = 0;
    do {
      vec_const_char lineview = str;
      lineview.len = nl - str.data;
      vec_append_vec(&m->buffer->lines.data[m->y + ev.len], &lineview);

      // insert next line
      ev.len++;
      vec_char line;
      vec_init(&line);
      vec_insert(&m->buffer->lines, m->y + ev.len, line);

      str.len -= nl - str.data + 1;
      str.data = nl + 1;
    } while ((nl = memchr(str.data, '\n', str.len)) != NULL);

    vec_append_vec(&m->buffer->lines.data[m->y + ev.len], &str);
    vec_append_vec(&m->buffer->lines.data[m->y + ev.len], &last_line);
    ev.len++;
    if (!keep_pos) m->x = str.len;
  }

  if (!keep_pos) {
    m->y += ev.len - 1;
  }
  dispatch_event((struct Event){event_edit, .edit = &ev});
}
