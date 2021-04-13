#include <stddef.h>
#include "draw/highlight_cursor.h"

Highlighter highlighters[] = { /* buffer_highlight, move_highlight*/ highlight_cursor, NULL};

void highlight_line(struct Buffer *buffer, size_t y, vec_style *style) {
  for (int i=0; highlighters[i] != NULL; i++) {
    highlighters[i](buffer, y, style);
  }
}
