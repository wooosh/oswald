#include <meraki/input.h>
#include <meraki/term.h>

#include <buffer/buffer.h>
#include <buffer/open.h>
#include <buffer/mark.h>

#include <event.h>
#include <x.h>

#include "adt/vec.h"

#include <unistd.h>
#include <stdio.h>

#include <main.h>

struct Editor E;

// TODO: add unit tests with --test <test_case> argument
// TODO: rename vec_push to vec_append

static void restore_term() {
  meraki_term_restore(E.term);
}

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("requires one or more filename arguments\n");
    return 1;
  }

  E.term = meraki_term_create();
  if (!meraki_term_raw(E.term)) {
    fprintf(stderr, "Cannot initialize terminal.\n");
    return 1;
  }

  atexit(restore_term);

  argv++;
  argc--;
  for (int i=0; i<argc; i++) {
    struct Buffer *b = buffer_open_file(argv[i]);

    if (b == NULL) {
      // TODO: use editor error message system
      fatal("could not open file %s\n", argv[i]);
    }

    vec_push(&E.buffers, b);
    E.cursor.buffer = b;
    E.anchor.buffer = b;
    E.cursor.x++;
    
    dispatch_event((struct Event){event_open, .open = b});
  } 
/*
  while (true) {
    struct Key key = term_read_key();
    if (key.base == 'q' && key.control) break;
        fprintf(stderr, "%d\n", key.base);

    if (!key.control && !key.alt) {
      switch (key.base) {
      case KeyBackspace:
        if (E.cursor.x == E.anchor.x && E.cursor.y == E.anchor.y) {
          if (E.cursor.x != 0) {
            E.anchor.x--;
          } else {
            // TODO:
          }
        }
        mark_delete(&E.anchor, &E.cursor);
        // TODO: mark_cmp function
        break;
      case KeyLeftArrow:
        mark_move_rel(&E.cursor, -1, 0);
        break;
      case KeyRightArrow:
        mark_move_rel(&E.cursor, 1, 0);
        break;
      case KeyUpArrow:
        mark_move_rel(&E.cursor, 0, -1);
        break;
      case KeyDownArrow:
        mark_move_rel(&E.cursor, 0, 1);
        break;
      default: {
        vec_const_char v = {&key.base, 1, 1};
        mark_insert(&E.cursor, v, false);
        break;
      }
      }
    }

    // don't keep selection if shift isn't held down
    if (!key.shift) {
      E.anchor = E.cursor;
    }
  }
*/

  char c;
  read(STDIN_FILENO, &c, 1); 
  meraki_term_restore(E.term);
  return 0;
}
