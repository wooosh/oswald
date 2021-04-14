#pragma once

#include <adt/map.h>
#include <adt/vec.h>
#include <meraki/input.h>
#include <highlight.h>
#include <command.h>

struct Keybind {
  struct MerakiKey key;
  int argc;
  char **argv;
  char *mode;
};

/*
struct FileType {
  char *name;
  vec_str *suffixes;
  
  vec_keybind keybinds;
  Highlighter highlighter; 
}
*/

// TODO: replace vec of keybinds with a map
typedef vec_of(struct Keybind) vec_keybind;
struct Config {
  vec_keybind keybinds;
  // vec_style style
};

void load_config(struct Config *c);
