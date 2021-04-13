#pragma once

struct Keybind {
  struct MerakiKey key;
  Action action;
}

typedef vec_of(struct Keybind) vec_keybind;
struct FileType {
  char *name;
  vec_str *suffixes;
  
  vec_keybind keybinds;
  Highlighter highlighter; 
}

typedef vec_of(struct FileType) vec_filetype;
struct Config {
  vec_keybind keybinds;
  vec_filetype filetypes;
  colors;
}
