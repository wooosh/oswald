#pragma once
/*** includes ***/

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

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
int getCursorPosition(int *rows, int *cols);

// @Todo: rewrite to store window size in editor struct on window resize
int getWindowSize(int *rows, int *cols);
} // namespace Terminal
