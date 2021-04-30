#include "event.h"
#include "highlight.h"
#include "buffer/buffer.h"

void treesitter_update(struct Event e);
void treesitter_highlight(struct Buffer *buffer, size_t line, vec_style *highlight);
