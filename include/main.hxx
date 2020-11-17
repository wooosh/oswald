#pragma once

#include "row.hxx"
#include <vector>
#include <termios.h>

// TODO: rename to something that isn't editorconfig
// TODO: cx, cy invariants cx/cy >= 0 && cy <= numrows && cx <= current row length
extern struct editorConfig {
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