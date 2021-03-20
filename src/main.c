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
  if (argc != 2) {
    printf("requires one filename argument\n");
    return 1;
  }

  term_setup();

  struct Buffer *b = buffer_open_file(argv[1]);

  if (b == NULL) {
    fatal("could not open file %s\n", argv[1]);
  }

  //e.cursor.buffer = b;

  dispatch_event((struct Event){event_open, .open = b});

  term_read_key();

  term_restore();
  return 0;
}
