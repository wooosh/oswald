#include <main.h>

void register_commands(struct Command *commands) {
  for (int i=0; commands[i].name != NULL; i++) {
    map_set(&E.commands, commands[i].name, commands[i]);
  }
}
