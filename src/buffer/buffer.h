#pragma once

#include "adt/vec.h"

typedef vec_of(vec_char) vec_line;
struct Buffer {
  void *payload;

  void (*save)(struct Buffer*); 
  void (*close)(struct Buffer*);

  vec_line lines;
  vec_char title;
  // TODO|FEATURE: filetype
};
