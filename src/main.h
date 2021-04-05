#pragma once

#include <meraki/term.h>

#include "buffer/buffer.h"
#include "buffer/mark.h"
#include "adt/vec.h"

typedef vec_of(struct Buffer*) vec_buffer;

extern struct Editor {
  
  vec_buffer buffers;

  struct MerakiTerm *term;

  size_t screen_width;
  size_t screen_height;

  struct Mark cursor;
  // the anchor refers to the end of the selection
  struct Mark anchor;
} E;
