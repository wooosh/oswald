#pragma once
#include "row.hxx"

#include <termios.h>
#include <vector>

// TODO: consider using namespaced globals instead of struct
extern struct Editor {
  size_t cx, cy;
  size_t rx;
  size_t rowoff;
  size_t coloff;
  size_t screenrows;
  size_t screencols;
  std::vector<erow> row;
  bool dirty;
  // TODO: change filename and statusmsg to string
  char *filename;
} E;
