#pragma once
#include <stdbool.h>

struct Command {
  char *name;
  void (*fn)(void *payload, int argc, char **argv);
  void *payload;
  bool changes_selection;
};

void register_commands(struct Command *commands);
