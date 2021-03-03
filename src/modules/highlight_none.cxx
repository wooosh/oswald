#include "buffer/buffer.hxx"
#include "main.hxx"

void highlightNone(BufferEditEvent ev) {
  if (ev.type == BufferEditEvent::Removed) {
    return;
  }

  for (size_t i = ev.line; i < ev.line + ev.length; i++) {
    ev.buffer->rows[i].hl.assign(line.length(), HighlightType::Normal);
  }
}
