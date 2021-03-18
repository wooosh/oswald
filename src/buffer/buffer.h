#pragma once

#include <vec/vec.h>

enum HighlightType {
  HLNormal = 0,
  HLSelection,
  HLType
};

// TODO|CLEANUP: make vectors capital or everything else snake case
typedef vec_t(enum HighlightType) vec_highlight_type;
struct Line {
  vec_char_t contents;
  vec_highlight_type highlight;
};

typedef vec_t(struct Line) vec_line;
struct Buffer {
  vec_line lines;
  vec_char_t title;

  struct Buffer *prev;
  struct Buffer *next;
  // TODO|FEATURE: filetype
};

struct BufferList {
  struct Buffer *start;
  struct Buffer *end;
};

void buffer_list_append(struct BufferList *bl, struct Buffer *b);
