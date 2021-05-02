#include <command.h>
#include <stddef.h>
#include <main.h>
#include <x.h>

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

#include <dlfcn.h>
static void load_plugin(void *payload, int argc, char **argv) {
  // TODO: check argc
  void *handle = dlopen(argv[1], RTLD_NOW);
  if (handle == NULL) {
    // TODO: handle
    errmsg("Couldn't open so: %s\n", dlerror());
    return;
  }

  void (*plugin_init)();
  *(void**)(&plugin_init) = dlsym(handle, "plugin_init");
  if (plugin_init == NULL) {
    // TODO: better error message and nonfatal
    errmsg("Couldn't open plugin_init in so: %s\n", dlerror());
    return;
  }

  plugin_init();
}

static struct Command commands[] = {
  {"enter-mode",  enter_mode, NULL,  false},
  {"save-file", save_file, NULL, false},
  {"backspace",  backspace, NULL,  false},
  {"plugin-load", load_plugin, NULL, false},

//  {"quit",    move, (void*) MoveUp,    false},
  {NULL}
};

void p_misc() {
  register_commands(commands);
}
