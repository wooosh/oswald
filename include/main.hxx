#pragma once
#include "row.hxx"

#include <vector>

// TODO: rename this file
// TODO: consider using namespaced globals instead of struct
extern struct Editor {
  size_t cx, cy;
  // TODO: move these to draw.cxx 
  size_t rx;
  size_t rowoff;
  size_t coloff;
  // TODO: move these to terminal.cxx
  size_t screenrows;
  size_t screencols;

  std::vector<erow> row;
} E;
