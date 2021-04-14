#include <command.h>
#include <stddef.h>
#include <main.h>

static void enter_mode(void *payload, int argc, char **argv) {
  // TODO: handle wrong amount of args
  vec_truncate(&E.mode, 0);
  vec_append_str(&E.mode, argv[1]);
}

static struct Command commands[] = {
  {"enter-mode",  enter_mode, NULL,  false},
//  {"quit",    move, (void*) MoveUp,    false},
//  {"backspace",  move, (void*) MoveLeft,  false},
  {NULL}
};

void p_misc() {
  register_commands(commands);
}
