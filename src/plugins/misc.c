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

static struct Command commands[] = {
  {"enter-mode",  enter_mode, NULL,  false},
  {"save-file", save_file, NULL, false},
//  {"quit",    move, (void*) MoveUp,    false},
//  {"backspace",  move, (void*) MoveLeft,  false},
  {NULL}
};

void p_misc() {
  register_commands(commands);
}
