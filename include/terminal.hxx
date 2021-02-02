#pragma once

#include "main.hxx"
#include <ostream>

#define CTRL_KEY(k) ((k)&0x1f)
struct key {
  enum keyBase{
    Backspace = 127,
    LeftArrow = 1000,
    RightArrow,
    UpArrow,
    DownArrow,
    Delete,
    Home,
    End,
    PageUp,
    PageDown,
    // Internal use only
    None
  } base;
  bool control;
  bool alt;
  bool shift;
};

namespace Terminal {
void die(const char* msg);
void setup();
key readKey();

void setCursorPosition(std::ostream &out, size_t row, size_t col);
int getCursorPosition(size_t *rows, size_t *cols);

extern const std::string clearToRight;

extern const std::string homeCursor;
extern const std::string hideCursor;
extern const std::string showCursor;

// @Todo: rewrite to store window size in editor struct on window resize
int getWindowSize(size_t *rows, size_t *cols);
} // namespace Terminal
