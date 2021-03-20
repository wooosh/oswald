#include <term/input.h>
#include <term/mode.h>

#include <buffer/buffer.h>
#include <buffer/open.h>

#include <event.h>
#include <x.h>

#include <stdio.h>

#include <main.h>

struct Editor E;

// TODO: add unit tests with --test <test_case> argument

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("requires one or more filename arguments\n");
    return 1;
  }

  term_setup();

  argv++;
  argc--;
  for (int i=0; i<argc; i++) {
    struct Buffer *b = buffer_open_file(argv[i]);

    if (b == NULL) {
      // TODO: use editor error message system
      fatal("could not open file %s\n", argv[i]);
    }

    buffer_list_append(&E.buffers, b);
    dispatch_event((struct Event){event_open, .open = b});
  } 
 
  //e.cursor.buffer = b;
  term_read_key();

  term_restore();
  return 0;
}
