#pragma once

#include "adt/vec.h"

struct Line {
  vec_char contents;
};

typedef vec_of(struct Line) vec_line;
struct Buffer {
  vec_line lines;
  vec_char title;
  // TODO|FEATURE: filetype
};
