#pragma once

#include <buffer/buffer.h>
#include <stddef.h>

enum EventType {
  // event_open is triggered when a new buffer initialized with text is created
  event_open,
  event_edit,
  // CursorMove,
  // SelectionChange,
  // BufferClose
};

struct Event {
  enum EventType type;
  union {
    struct EditEvent *edit;
    struct Buffer *open;
  };
};

void dispatch_event(struct Event e);

struct EditEvent {
  struct Buffer *buffer;

  size_t start;
  size_t len;

  enum {
    Insert,
    Delete,
    Changed,
  } type;
};
