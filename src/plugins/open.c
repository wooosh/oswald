#include "buffer/buffer.h"
#include "x.h"
#include "main.h"
#include "event.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: detect changes between saves
void buffer_save_file(struct Buffer *buf) {
  //FILE *file = fopen(buf->title, "w");
  // TODO: handle fail to open
}

// TODO|CLEANUP: move this elsewhere?
void buffer_destroy(struct Buffer *buf) {
  vec_destroy(&buf->title);

  for (size_t i = 0; i < buf->lines.len; i++) {
    vec_destroy(&buf->lines.data[i]);
  }

  vec_destroy(&buf->lines);
  free(buf);
}

// Returns NULL when the file cannot be opened
// TODO|FEATURE: return proper error messages
void buffer_open_file(void *payload, int argc, char **argv) {
  // TODO: proper arg handling
  char *path = argv[1];
  struct Buffer *buf = xmalloc(sizeof(struct Buffer));
  buf->save = buffer_save_file;
  buf->close = buffer_destroy;
  vec_init(&buf->lines);

  vec_init(&buf->title);
  vec_append_str(&buf->title, path);

  FILE *file = fopen(path, "r");
  // TODO: handle fail to open by creating file

  while (true) {
    vec_char line;
    vec_init(&line);

    // TODO: support empty files
    // TODO|FEATURE: support other line endings
    //  to support other line endings, we will store a line_ending char[2] in
    //  the buffer struct and add the line ending during saving

    ssize_t len = getline(&line.data, &line.cap, file);
    // TODO|BUG: getline can return -1 on eof AND error, so we need to check for
    // errors as well
    if (len == -1)
      break;

    line.len = len;
    // newlines should not be included in our internal representation
    if (line.data[len - 1] == '\n') {
      line.len--;
    }

    vec_push(&buf->lines, line);
  }

  fclose(file);

  vec_push(&E.buffers, buf);
  E.cursor.buffer = buf;
  E.anchor.buffer = buf;

  dispatch_event((struct Event){event_open, .open = buf});
}

static struct Command commands[] = {
  {"buffer-open-file", buffer_open_file, NULL, false},
  {NULL}
}; 

void p_open() {
  register_commands(commands);
}
