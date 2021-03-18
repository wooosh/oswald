#pragma once

#include <stddef.h>

#include <buffer/buffer.h>
#include <vec/vec.h>
#include <stdbool.h>

struct Mark { 
  struct Buffer* buffer;

  size_t x;
  size_t y;
};

// negative = left/up
// positive = right/down
void mark_move_rel(struct Mark *m, ssize_t x, ssize_t y);

// deletes text between a and b
void mark_delete(struct Mark *a, struct Mark *b);

// if keep_pos is false, then the cursor will insert the selection behind the
// cursor
void mark_insert(struct Mark *m, vec_const_char_t str, bool keep_pos);

// returns true if a's position <= b's position
bool mark_order(struct Mark *a, struct Mark *b);
