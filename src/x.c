#include <x.h>
#include <term/mode.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <errno.h>

size_t min_size_t(size_t a, size_t b) {
  return a < b ? a : b;
} 

size_t max_size_t(size_t a, size_t b) {
  return a > b ? a : b;
} 

ssize_t min_ssize_t(ssize_t a, ssize_t b) {
  return a < b ? a : b;
} 

ssize_t max_ssize_t(ssize_t a, ssize_t b) {
  return a > b ? a : b;
} 

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

