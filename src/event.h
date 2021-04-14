#pragma once

#include <buffer/buffer.h>
#include <buffer/mark.h>
#include <stddef.h>

enum EventType {
  // event_open is triggered when a new buffer initialized with text is created
  event_open,
  event_edit,
  // TODO: replace with cursor move
  event_mark_move,
  event_render,
  // CursorMove,
  // SelectionChange,
  // BufferClose
};

struct Event {
  enum EventType type;
  union {
    struct EditEvent *edit;
    struct Buffer *open;
    struct Mark *mark_move;
  };
};

void dispatch_event(struct Event e);

struct EditEvent {
  struct Buffer *buffer;

  size_t start;
  size_t len;

  enum {
    EditInsert,
    EditDelete,
    EditChanged,
  } type;
};
