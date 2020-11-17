#pragma once

#include "row.hxx"

#include <termios.h>
#include <vector>

extern struct Editor {
  size_t cx, cy;
  size_t rx;
  size_t rowoff;
  size_t coloff;
  size_t screenrows;
  size_t screencols;
  std::vector<erow> row;
  bool dirty;
  char *filename;
  char statusmsg[80];
  time_t statusmsg_time;
  // TODO: make static variable in terminal.cpp
  struct termios orig_termios;
} E;

void editorSetStatusMessage(const char *fmt, ...);