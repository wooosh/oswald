#include "adt/map.h"
#include "adt/vec.h"
#include "command.h"
#include "main.h"
#include "buffer/mark.h"

// TODO: load from disk
// TODO: allow tabbing through fields in macros

typedef map_t(vec_const_char) map_const_str;
static map_const_str snippets;

// TODO: editor error system
void snippet_insert(void *payload, int argc, char **argv) {
  if (E.cursor.buffer) {
    // TODO: test for argv 1 and snippet existing
    vec_const_char *snippet = map_get(&snippets, argv[1]);
    if (snippet) {
      mark_insert(&E.cursor, *snippet, false);
    } 
  }
}

static struct Command commands[] = {
  {"snippets-insert", snippet_insert, NULL, false},
  // {"snippets-reload", reload, NULL, false}
  {NULL}
};

void p_snippets() {
  map_init(&snippets);

  map_set(&snippets, "test",
    vec_from_str("hello\nworld"));
  map_set(&snippets, "cmain",
    vec_from_str("int main(int argc, char **argv) {\nreturn 0;\n}"));

  register_commands(commands);
}
