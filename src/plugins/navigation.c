#include <command.h>
#include <stddef.h>
#include <main.h>
#include <buffer/mark.h>
#include <stdio.h>

enum MoveDirection {
  MoveUp,
  MoveDown,
  MoveLeft,
  MoveRight,
};

// TODO: check if mark is in buffer

// move <up|down|left|right> [distance]
static void move(void *payload, int argc, char **argv) {
  // TODO: support distance
  enum MoveDirection dir = (enum MoveDirection) payload;
  switch(dir) {
    case MoveUp:
     mark_move_rel(&E.cursor, 0, -1);
     break; 
    case MoveDown:
     mark_move_rel(&E.cursor, 0, 1);
     break; 
    case MoveLeft:
     mark_move_rel(&E.cursor, -1, 0);
     break; 
    case MoveRight:
     mark_move_rel(&E.cursor, 1, 0);
     break;
    // TODO: handle error case
  }
}

enum LineDirection {LineStart, LineEnd};

static void line(void *payload, int argc, char **argv) {

}
 
static struct Command commands[] = {
  {"move-up",    move, (void*) MoveUp,    false},
  {"move-down",  move, (void*) MoveDown,  false},
  {"move-left",  move, (void*) MoveLeft,  false},
  {"move-right", move, (void*) MoveRight, false},

  {"line-start", line, (void*) LineStart, false},
  {"line-end",   line, (void*) LineEnd,   false},
  {NULL}
};

void p_navigation() {
  register_commands(commands);
}
