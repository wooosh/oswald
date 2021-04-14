#include <main.h>
#include <imp.h>
#include <x.h>
#include <config.h>

struct KeyName {
  char* name;
  enum MerakiKeyBase key;
};

struct KeyName keynames[] = {
  {"backspace", MerakiBackspace},
  {"delete", MerakiDelete},
  {"left", MerakiLeftArrow},
  {"right", MerakiRightArrow},
  {"up", MerakiUpArrow},
  {"down", MerakiDownArrow},
  {"home", MerakiHome},
  {"end", MerakiEnd},
  {"pgup", MerakiPageUp},
  {"pgdown", MerakiPageDown},
  {NULL, MerakiKeyNone}
};


// returns KeyNone if no match
struct MerakiKey parse_key(char *str) {
  // TODO: check if key is valid and/or conflicts
  // TODO: handle dashes
  // TODO: parse modifiers
  struct MerakiKey k = {MerakiKeyNone, false, false, false};
  
  // if single char use character code
  if (str[1] == NULL) {
    k.base = str[0];
  } else {
    for (int i=0; keynames[i].name != NULL; i++) {
      if (strcmp(keynames[i].name, str) == 0) {
        k.base = keynames[i].key;
        break;
      }
    }
  }

  return k;
}

void directive_handler(void *payload, size_t line, size_t argc, char **argv) {
  struct Config *c = payload;
  if (argc < 2)
    fatal("Expected atleast one directive and one argument on line %zu\n", line);

  if (strcmp(argv[0], "keybind") == 0) {
    struct MerakiKey k = parse_key(argv[1]);
    if (k.base == MerakiKeyNone)
      fatal("Could not parse key '%s' on line %zu\n", argv[1], line);

    argc -= 2;
    argv += 2;
    char **nargv = malloc((argc+1)*sizeof(char*));
    for (int i=0; i<argc; i++) {
      nargv[i] = strdup(argv[i]);
    }
    nargv[argc] = NULL;
    struct Keybind kb = {
      k,
      argc,
      nargv,
    };

    vec_push(&c->keybinds, kb);
  } else {
    fatal("Unknown directive on line %zu\n", argv[1], line);
  }
}

void load_config(struct Config *c) {
  FILE *f = fopen("config.imp", "r");
  if (f == NULL)
    // TODO: replace xasserts with fatals in most places
    fatal("Could not open config.imp\n");

  vec_init(&c->keybinds);

  struct imp_parser imp = imp_init(directive_handler, c, f);
  // TODO: handle errors
  while (imp_next_command(&imp) == imp_success) {};
}
