#include <term/mode.h>
#include <term/input.h>

#include <buffer/buffer.h>
#include <buffer/open.h>

#include <x.h>

#include <stdio.h>

// TODO: add unit tests with --test <test_case> argument

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("requires one filename argument\n");
    return 1;
  }

  struct Buffer* b = buffer_open_file(argv[1]);

  if (b == NULL) {
    printf("could not open file %s\n", argv[1]);
    return 1;
  }

  for (int i=0; i<b->lines.length; i++) {
    printf("line %d: %s\n", i, b->lines.data[i].contents.data);
  }

  return 0;
}
