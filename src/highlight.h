#pragma once

#include <stddef.h>

#include <meraki/output.h>

#include "buffer/buffer.h"

typedef vec_of(struct MerakiStyle) vec_style;
typedef void (*Highlighter)(struct Buffer *buffer, size_t y, vec_style *style);

// style array should be the same length as the contents of the line and have
// all values initialized to the default style
void highlight_line(struct Buffer *buffer, size_t y, vec_style *style);
