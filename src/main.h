#pragma once

#include <buffer/buffer.h>

extern struct Editor {
  vec_t(Buffer) buffers;

  Mark cursor;
  // the anchor refers to the end of the selection
  Mark anchor;
} E;
