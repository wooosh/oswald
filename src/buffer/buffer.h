#pragma once

#include <vec/vec.h>

enum HighlightType {
  HLNormal = 0,
  HLSelection,
  HLType
};

struct Line {
  vec_char_t contents;
  vec_t(enum HighlightType) highlight;
};

struct Buffer {
  vec_t(struct Line) lines;
  vec_char_t title;

  struct Buffer *prev;
  struct Buffer *next;
  // TODO: refcount for marks/iterators/handles
  // TODO: filetype
};

struct BufferList {
  struct Buffer *start;
  struct Buffer *end;
};

void buffer_list_append(struct BufferList *bl, struct Buffer *b);
