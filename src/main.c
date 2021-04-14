#include <meraki/input.h>
#include <meraki/term.h>

#include <buffer/buffer.h>
#include <buffer/open.h>
#include <buffer/mark.h>

#include <event.h>
#include <x.h>
#include "plugins.h"

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
  E.term = meraki_term_create();

  map_init(&E.commands);

  // init plugins
  for (int i=0; plugins[i] != NULL; i++) {
    plugins[i]();
  }
  
  load_config(&E.config);

  if (argc < 2) {
    printf("requires one or more filename arguments\n");
    return 1;
  }

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
    
    dispatch_event((struct Event){event_open, .open = b});
  } 
 
  struct MerakiInput *mi = meraki_term_input(E.term); 
  while (true) {
    fprintf(stderr, "%zu %zu\n", E.cursor.x, E.cursor.y);
    struct MerakiKey key = meraki_read_key(mi);
    if (key.base == 'q' && key.control) break;
    for (int i=0; i<E.config.keybinds.len; i++) {
      struct Keybind kb = E.config.keybinds.data[i];
      fprintf(stderr, "%zu %zu\n", kb.key.base, key.base);
      if (key.base == kb.key.base) {
        fprintf(stderr, "%s\n", kb.argv[0]);
        struct Command *c = map_get(&E.commands, kb.argv[0]);
        if (c) {
          fprintf(stderr, "test\n");
          c->fn(c->payload, kb.argc - 1, kb.argv + 1);
        }
        break;
      }
    }

    // don't keep selection if shift isn't held down
    if (!key.shift) {
      E.anchor = E.cursor;
    }
    dispatch_event((struct Event){event_render});
  }

  meraki_term_restore(E.term);
  return 0;
}
