#pragma once
#include <list>

#include "buffer/buffer.hxx"

// TODO: move into buffer/
struct Mark {
  // TODO: rename p to buffer
  std::list<Buffer>::iterator p;
  size_t x;
  size_t y;

  struct Row *row();
  void moveRight();
  void moveLeft();
  void moveUp();
  void moveDown();

  void deleteBackward();
  void insertChar(char c);
  // void insertString(std::string s);
  // void deleteRange();
};

// can probably rename this to EditEvent
struct BufferEditEvent {
  // first line of edit
  size_t line;
  // number of lines edited
  size_t length;

  std::list<Buffer>::iterator buffer;

  enum {
    Removed,
    Inserted,
    Changed
  } type;
};

struct Event {
  enum {
    BufferEdit,
    CursorMove,
    SelectionChange,
    BufferOpen
    // BufferClose
  } type;
  union {
    BufferEditEvent bufferEdit;
    std::list<Buffer>::iterator bufferOpen;
  };
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

  std::list<Event> events;
  //std::list<Event>::iterator processed;

  Mark cursor;
  Mark anchor;

  // TODO: rename to bufferList or buffers
  std::list<Buffer> portions;
} E;
