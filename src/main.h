#pragma once

#include <buffer/buffer.h>
#include <buffer/mark.h>

extern struct Editor {
  BufferList bl;

  Mark cursor;
  // the anchor refers to the end of the selection
  Mark anchor;
} E;
