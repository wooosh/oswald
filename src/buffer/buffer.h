#pragma once

#include <vec.h>

struct Mark { 
  Buffer* b;

  size_t row;
  size_t col;
};

enum HighlightType {
  Normal,
  Selection,
  Type
};

struct Row {
  vec_char_t contents;
  vec_t(HighlightType) highlight;
};

struct Buffer {
  vec_t(Row) rows;
  vec_char_t title;

  // TODO: refcount for marks/iterators/handles
  // TODO: filetype
};
