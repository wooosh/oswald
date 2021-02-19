#pragma once
#include <list>
#include <string>
#include <vector>

// TODO: rename to row
typedef struct erow {
  std::string raw;
  std::string render;
  bool dirty;

  // TODO: don't export this function, just redraw rows when they are set to
  // dirty
  void updateRender();
} erow;

typedef struct portion {
  // TODO: change to std::filesystem::path
  std::string filename;
  std::vector<erow> rows;
  // TODO: method to create a mark
} portion;

typedef struct mark {
  std::list<struct portion>::iterator p;
  size_t x;
  size_t y;

  struct erow *row();
  void moveRight();
  void moveLeft();
  void moveUp();
  void moveDown();
  void deleteBackward();
  void insertChar(char c);
} mark;

// TODO: rename this file
// TODO: consider using namespaced globals instead of struct
extern struct Editor {
  // TODO: screen struct
  // TODO: buffer struct
  // TODO: move these to draw.cxx
  size_t rx;
  size_t rowoff;
  size_t coloff;
  // TODO: move these to terminal.cxx
  size_t screenrows;
  size_t screencols;

  struct mark cursor;
  struct mark anchor;

  // TODO: change to doubly linked list
  std::list<portion> portions;

  // rename to buffer
  // std::vector<erow> row;
} E;
