#pragma once

#include "main.hxx"
#include "terminal.hxx"

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
int getCursorPosition(size_t *rows, size_t *cols);

// @Todo: rewrite to store window size in editor struct on window resize
int getWindowSize(size_t *rows, size_t *cols);
} // namespace Terminal
