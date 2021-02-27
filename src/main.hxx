#pragma once
#include <list>

#include "buffer/buffer.hxx"

// TODO: move into buffer/
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

// TODO: rename this file to global or something
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
} E;
