#pragma once

#include <meraki/term.h>

#include "buffer/buffer.h"
#include "buffer/mark.h"
#include "highlight.h"
#include "command.h"
#include <config.h>
#include "adt/vec.h"
#include "adt/map.h"

typedef vec_of(struct Buffer*) vec_buffer;
typedef map_t(Highlighter) map_highlight;
typedef map_t(struct Command) map_command; 

extern struct Editor {
  vec_buffer buffers;

  struct MerakiTerm *term;

  size_t screen_width;
  size_t screen_height;

  // TODO: array of cursors
  // TODO: keep copy buffer for each cursor
  // TODO: make cursors not always be a selection
  struct Mark cursor;
  // the anchor refers to the end of the selection
  struct Mark anchor;

  map_highlight highlighters;
  map_command commands;

  vec_char mode;

  struct Config config;
} E;
