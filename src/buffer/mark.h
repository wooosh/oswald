#pragma once

#include <sys/types.h>

#include <adt/vec.h>
#include <buffer/buffer.h>
#include <stdbool.h>

// read only struct
struct Mark {
  struct Buffer *buffer;

  size_t x;
  size_t y;
};

// void mark_move_to

// negative = left/up
// positive = right/down
void mark_move_rel(struct Mark *m, ssize_t x, ssize_t y);

// deletes text between a and b
void mark_delete(struct Mark *a, struct Mark *b);

// if keep_pos is false, then the cursor will insert the selection behind the
// cursor
void mark_insert(struct Mark *m, vec_const_char str, bool keep_pos);

// returns true if a's position <= b's position
bool mark_order(struct Mark *a, struct Mark *b);
