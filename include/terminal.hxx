#pragma once

#include "main.hxx"
#include <ostream>

#define CTRL_KEY(k) ((k)&0x1f)
enum editorKey {
  BACKSPACE = 127,
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};

namespace Terminal {
void die(const char *s);
void disableRawMode();
void enableRawMode();
int readKey();

// TODO: probably should not use ostringstream here, could probably be ostream
void setCursorPosition(std::ostream &out, size_t row, size_t col);
int getCursorPosition(size_t *rows, size_t *cols);

extern const std::string clearLine;

extern const std::string homeCursor;
extern const std::string hideCursor;
extern const std::string showCursor;

// @Todo: rewrite to store window size in editor struct on window resize
int getWindowSize(size_t *rows, size_t *cols);
} // namespace Terminal
