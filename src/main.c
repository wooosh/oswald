#include <term/mode.h>
#include <term/input.h>

#include <buffer/buffer.h>
#include <buffer/open.h>

#include <adt/vec.h>

#include <draw/display_string.h>

#include <x.h>

#include <stdio.h>

// TODO: add unit tests with --test <test_case> argument

int main(int argc, char** argv) {
  vec_char str;
  vec_init(&str);

  vec_push(&str, 'a');
  vec_append_str(&str, "hello");

  for (size_t i=0; i<str.len; i++) {
    printf("%c", str.data[i]);
  }
  printf("len: %d\n", str.len);
  
  vec_fill(&str, 'a', 0, 5);

  for (size_t i=0; i<str.len; i++) {
    printf("%c", str.data[i]);
  }
  printf("len: %d\n", str.len);

  //vec_splice(&str, 1, 5);
  vec_append_str(&str, "hello");

  for (size_t i=0; i<str.len; i++) {
    printf("%c", str.data[i]);
  }
  printf("\n");
  vec_destroy(&str);

  /*
  if (argc != 2) {
    printf("requires one filename argument\n");
    return 1;
  }

  struct Buffer* b = buffer_open_file(argv[1]);

  if (b == NULL) {
    printf("could not open file %s\n", argv[1]);
    return 1;
  }

  //for (int i=0; i<b->lines.length; i++) {
  //  size_t l = ds_len_display_to_raw(b->lines.data[i].contents, 3);
  //  printf("line %zu, %d: %s\n", l, i, b->lines.data[i].contents.data);
  //}
  */

  return 0;
}
