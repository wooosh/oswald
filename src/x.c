#include <x.h>
#include <term/mode.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>

// TODO: make sure vec uses xmalloc/assert

void xassert_(bool condition, char* message, char* file, size_t line) {
  if (!condition) {
    term_restore();

    fprintf(stderr, "Assertion failed at %s:%zu '%s'\n", file, line, message);
    exit(1);
  }
}

void xassert_errno_(bool condition, char* message, char* file, size_t line) {
  if (!condition) {
    int saved_errno = errno;
    term_restore();
    errno = saved_errno;
    fprintf(stderr, "Assertion failed at %s:%zu: ", file, line);
    perror(message);
  }
}

// exits on failed allocation
// TODO: save work on failed allocation
void* xmalloc(size_t size) {
  void* ptr = malloc(size);
  xassert_errno(ptr != NULL, NULL);
  return ptr;
}

