#include <buffer/buffer.h>
#include <x.h>

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// TODO|CLEANUP: move this elsewhere?
void buffer_destroy(struct Buffer *buf) {
  vec_destroy(&buf->title);

  for (size_t i = 0; i < buf->lines.len; i++) {
    vec_destroy(&buf->lines.data[i].contents);
    vec_destroy(&buf->lines.data[i].highlight);
  }

  vec_destroy(&buf->lines);
  free(buf);
}

// Returns NULL when the file cannot be opened
// TODO|FEATURE: return proper error messages
struct Buffer *buffer_open_file(char *path) {
  struct Buffer *buf = xmalloc(sizeof(struct Buffer));
  vec_init(&buf->lines);

  vec_init(&buf->title);
  vec_append_str(&buf->title, path);

  FILE *file = fopen(path, "r");
  if (file == NULL) {
    return NULL;
  }

  while (true) {
    struct Line line;
    vec_init(&line.contents);

    // TODO|FEATURE: support other line endings
    //  to support other line endings, we will store a line_ending char[2] in
    //  the buffer struct and add the line ending during saving

    ssize_t len = getline(&line.contents.data, &line.contents.cap, file);
    // TODO|BUG: getline can return -1 on eof AND error, so we need to check for
    // errors as well
    if (len == -1)
      break;

    line.contents.len = len;
    // newlines should not be included in our internal representation
    if (line.contents.data[len - 1] == '\n') {
      line.contents.len--;
    }

    vec_init(&line.highlight);
    struct MerakiStyle s = {{Meraki8Color, -1}, {Meraki8Color, -1}, MerakiNone};
    vec_fill(&line.highlight, s, 0, len);

    vec_push(&buf->lines, line);
  }

  fclose(file);

  return buf;
}

// TODO|FEATURE: buffer_open_scratch
// TODO|FEATURE: buffer_open_portion
//  - Can we use pointers to other lines to synchronize lines?
