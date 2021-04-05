#include <meraki/term.h>

#include <main.h>
#include <x.h>

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

size_t min_size_t(size_t a, size_t b) { return a < b ? a : b; }

size_t max_size_t(size_t a, size_t b) { return a > b ? a : b; }

ssize_t min_ssize_t(ssize_t a, ssize_t b) { return a < b ? a : b; }

ssize_t max_ssize_t(ssize_t a, ssize_t b) { return a > b ? a : b; }

void xassert_(bool condition, char *message, char *file, size_t line) {
  if (!condition) {
    meraki_term_restore(E.term);

    fprintf(stderr, "Assertion failed at %s:%zu '%s'\n", file, line, message);
    exit(1);
  }
}

void xassert_errno_(bool condition, char *message, char *file, size_t line) {
  if (!condition) {
    int saved_errno = errno;
    meraki_term_restore(E.term);
    errno = saved_errno;
    fprintf(stderr, "Assertion failed at %s:%zu: ", file, line);
    perror(message);
  }
}

// TODO|FEATURE: printf specifiers
void fatal(const char *format, ...) {
  va_list args;
  va_start(args, format);

  meraki_term_restore(E.term);
  vfprintf(stderr, format, args);

  exit(1);
}

// exits on failed allocation
// TODO: save work on failed allocation
void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  xassert_errno(ptr != NULL, NULL);
  return ptr;
}

void *xrealloc(void *ptr, size_t size) {
  ptr = realloc(ptr, size);
  xassert_errno(ptr != NULL, NULL);
  return ptr;
}
