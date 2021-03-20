#pragma once

#include <buffer/buffer.h>
#include <buffer/mark.h>

extern struct Editor {
  struct BufferList bl;

  size_t screen_width;
  size_t screen_height;

  struct Mark cursor;
  // the anchor refers to the end of the selection
  struct Mark anchor;
} E;
