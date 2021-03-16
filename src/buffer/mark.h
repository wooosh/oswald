#pragma once

#include <buffer/buffer.h>

struct Mark { 
  Buffer* b;

  size_t row;
  size_t col;
}
