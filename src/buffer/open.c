#include <buffer/buffer.h>
#include <xmalloc.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// TODO: move this elsewhere?
void buffer_destroy(struct Buffer* buf) {
  vec_deinit(&buf->title);
  
  for (size_t i=0; i<buf->lines.length; i++) {
    vec_deinit(&buf->lines.data[i].contents);
    vec_deinit(&buf->lines.data[i].highlight);
  }

  vec_deinit(&buf->lines);
  free(buf);
}

// Returns NULL when the file cannot be opened
// TODO: return proper error messages
struct Buffer *buffer_open_file(char *path) {
  struct Buffer *buf = xmalloc(sizeof(struct Buffer));
  vec_init(&buf->lines);

  vec_init(&buf->title);
  vec_pushstr(&buf->title, path);

  FILE* file = fopen(path, "r");
  if (file == NULL) {
    return NULL;
  }
  
  while (true) {
    struct Line line;
    vec_init(&line.contents);

    // TODO: support other line endings
    //  to support other line endings, we will store a line_ending char[2] in
    //  the buffer struct and add the line ending during saving

    ssize_t len = getline(&line.contents.data, &line.contents.capacity, file);
    // TODO: getline can return -1 on eof AND error, so we need to check for
    // errors as well
    if (len == -1) break;
    
    line.contents.length = len;
    // newlines should not be included in our internal representation
    if (line.contents.data[len] == '\n') {
      line.contents.length--;
    }

    vec_fill(&line.highlight, HLNormal, len);
    
    vec_push(&buf->lines, line);
  }

  fclose(file);
  return buf;
}

// TODO: buffer_open_scratch
// TODO: buffer_open_portion
//  - Can we use pointers to other lines to synchronize lines?
