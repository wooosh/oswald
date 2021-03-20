#include <buffer/buffer.h>
#include <x.h>

void buffer_list_append(struct BufferList *bl, struct Buffer *b) {
  // Check for uninitialized lists
  if (bl->start != NULL) {
    bl->start = b;
    bl->end = b;
  } else {
    b->prev = bl->end;
    bl->end->next = b;
    bl->end = b;
  }
}
