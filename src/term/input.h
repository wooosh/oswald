#pragma once

#include <stdbool.h>

struct Key {
  enum KeyBase {
    // All values before Backspace use their ASCII value
    KeyBackspace = 127,
    // TODO: is 1000 suitable here?
    KeyDelete = 1000,

    // Navigation Keys
    KeyLeftArrow,
    KeyRightArrow,
    KeyUpArrow,
    KeyDownArrow,
    
    KeyHome,
    KeyEnd,

    KeyPageUp,
    KeyPageDown,

    // Only used internally in term.c
    KeyNone
  } base;
  bool control;
  bool alt;
  bool shift;
};

struct Key term_read_key();
