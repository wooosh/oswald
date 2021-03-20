#include <adt/vec.h>
#include <buffer/buffer.h>
#include <buffer/mark.h>
#include <x.h>

struct Line *mark_line(struct Mark *m) {
  return &m->buffer->lines.data[m->y];
}

// TODO: should we use size_t everywhere because it messes up signed comparisons
// clamps value to [min, max)
ssize_t clamp(ssize_t min, ssize_t max, ssize_t val) {
  if (val < min)
    return min;
  if (val > max)
    return max - 1;
  return val;
}

// TODO: mark_move that makes left on position zero move up a line, and right on
// position go down a line

// negative = left/up
// positive = right/down
void mark_move_rel(struct Mark *m, ssize_t x, ssize_t y) {
  m->y = clamp(0, m->buffer->lines.len, y);
  m->x = clamp(0, mark_line(m)->contents.len, x);
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

  struct Line *line_start = mark_line(start);
  struct Line *line_end = mark_line(end);

  // make sure marks are actually in order
  if (!mark_cmp(start, end)) {
    start = b;
    end = a;
  }

  // delete within single line
  if (start->y == end->y) {
    vec_splice(&line_start->contents, start->x, end->x);
    vec_splice(&line_start->highlight, start->x, end->x);
    return;
  }

  // multline deletes

  // delete start and end lines first because the positions will not have to be
  // adjusted when we remove the lines between
  vec_truncate(&line_start->contents, start->x);
  vec_truncate(&line_start->highlight, start->x);

  vec_splice(&line_end->contents, 0, end->x);
  vec_splice(&line_end->highlight, 0, end->x);

  vec_line *lines = &start->buffer->lines;
  // remove all of the lines in between
  for (size_t i = start->y + 1; i < end->y; i++) {
    // TODO: line_destroy() function
    vec_destroy(&lines->data[i].contents);
    vec_destroy(&lines->data[i].highlight);
  }

  vec_splice(lines, start->y + 1, end->y);
}

// TODO: handle multiline inserts
void mark_insert(struct Mark *m, vec_const_char str, bool keep_pos) {
  vec_insert_vec(&mark_line(m)->contents, &str, m->x);
  // TODO: vec_fill highlight string

  if (!keep_pos) {
    m->x += str.len;
  }
}
