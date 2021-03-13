#pragma once

#include <stdbool.h>

struct Key {
  enum KeyBase {
    // All values before Backspace use their ASCII value
    Backspace = 127,
    // TODO: is 1000 suitable here?
    Delete = 1000,

    // Navigation Keys
    LeftArrow,
    RightArrow,
    UpArrow,
    DownArrow,
    
    Home,
    End,

    PageUp,
    PageDown,

    // Only used internally in term.c
    None
  }
  bool control;
  bool alt;
  bool shift;
};

void term_read_key();
