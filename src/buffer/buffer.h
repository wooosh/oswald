#pragma once

#include "adt/vec.h"

typedef vec_of(vec_char) vec_line;
struct Buffer {
  vec_line lines;
  vec_char title;
  // TODO|FEATURE: filetype
};
