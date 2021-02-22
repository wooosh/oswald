#pragma once
#include <list>
#include <string>
#include <vector>

struct Row {
  std::string raw;
  std::string render;
  bool dirty;

  // TODO: don't export this function, just redraw rows when they are set to
  // dirty and need to be drawn
  void updateRender();
};

struct Buffer {
  // TODO: change to std::filesystem::path
  std::string filename;
  std::vector<Row> rows;
  // TODO: method to create a mark
};

struct Mark {
  std::list<struct Buffer>::iterator p;
  size_t x;
  size_t y;

  struct Row *row();
  void moveRight();
  void moveLeft();
  void moveUp();
  void moveDown();
  void deleteBackward();
  void insertChar(char c);
};

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

  Mark cursor;
  Mark anchor;

  // TODO: change to doubly linked list
  std::list<Buffer> portions;

  // rename to buffer
  // std::vector<erow> row;
} E;
