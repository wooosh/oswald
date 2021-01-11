#pragma once
#include <vector>
#include <string>

// TODO: rename to row
typedef struct erow {
  std::string raw;
  std::string render;
  bool dirty;

  // TODO: don't export this function, just redraw rows when they are set to dirty
  void updateRender();
} erow;

typedef struct portion {
  // TODO: change to std::filesystem::path
  std::string filename;
  std::vector<erow> rows;
  // TODO: method to create a mark
} portion;

// TODO: move to mark.hxx
typedef struct mark {
  struct portion* p;
  size_t x;
  size_t y;
} mark;

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

  // rename to buffer
  std::vector<erow> row;
} E;
