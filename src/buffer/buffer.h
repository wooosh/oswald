#pragma once

#include <adt/vec.h>

enum HighlightType {
  HLNormal = 0,
  HLSelection,
  HLType
};

typedef vec_of(enum HighlightType) vec_highlight_type;
struct Line {
  vec_char contents;
  vec_highlight_type highlight;
};

typedef vec_of(struct Line) vec_line;
struct Buffer {
  vec_line lines;
  vec_char title;

  struct Buffer *prev;
  struct Buffer *next;
  // TODO|FEATURE: filetype
};

struct BufferList {
  struct Buffer *start;
  struct Buffer *end;
};

void buffer_list_append(struct BufferList *bl, struct Buffer *b);
