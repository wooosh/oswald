#include "buffer/buffer.hxx"
#include "main.hxx"

extern "C" {
#include <tree_sitter/treesitter.h>
}

void highlightTreeSitter(BufferEditEvent ev) {
  if (ev.type == BufferEditEvent::Removed) {
    return;
  }

  for (size_t i = ev.line; i < ev.line + ev.length; i++) {
    Row *r = &ev.buffer->rows[i];
    r->hl.assign(r->raw.length(), HighlightType::Normal);
  }
}