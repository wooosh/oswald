#include <command.h>
#include <stddef.h>
#include <main.h>

static void enter_mode(void *payload, int argc, char **argv) {
  // TODO: handle wrong amount of args
  vec_truncate(&E.mode, 0);
  vec_append_str(&E.mode, argv[1]);
}

// saves the current file
static void save_file(void *payload, int argc, char **argv) {
  if (E.cursor.buffer == NULL) return;
  E.cursor.buffer->save(E.cursor.buffer);
}

static void backspace(void *payload, int argc, char **argv) {
  if (E.cursor.buffer == NULL) return;

  if (E.cursor.x == E.anchor.x && E.cursor.y == E.anchor.y) {
    if (E.cursor.y != 0 && E.cursor.x == 0) {
      E.anchor.y--;
      E.anchor.x = E.cursor.buffer->lines.data[E.anchor.y].len;
    } else if (E.cursor.x != 0) {
      E.anchor.x--;
    }
  }

  mark_delete(&E.anchor, &E.cursor);
}

static struct Command commands[] = {
  {"enter-mode",  enter_mode, NULL,  false},
  {"save-file", save_file, NULL, false},
  {"backspace",  backspace, NULL,  false},
//  {"quit",    move, (void*) MoveUp,    false},
  {NULL}
};

void p_misc() {
  register_commands(commands);
}
