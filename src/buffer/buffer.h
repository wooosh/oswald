#pragma once

#include <meraki/output.h>

#include "adt/vec.h"

typedef vec_of(struct MerakiStyle) vec_style;
struct Line {
  vec_char contents;
  vec_style highlight;
};

typedef vec_of(struct Line) vec_line;
struct Buffer {
  vec_line lines;
  vec_char title;
  // TODO|FEATURE: filetype
};
