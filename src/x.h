#pragma once

#include <stdbool.h>
#include <stddef.h>

#define min(X) _Generic((X), size_t : min_size_t, ssize_t : min_ssize_t, )(X)

#define max(X) _Generic((X), size_t : max_size_t, ssize_t : max_ssize_t, )(X)

// exits on failed allocation
void *xmalloc(size_t size);
void *xrealloc(void *ptr, size_t size);

// TODO: make assert do nothing by defining it to ';' when NDEBUG is on
// message must be provided
#define xassert(condition, message)                                            \
  xassert_((condition), (message), __FILE__, __LINE__);

// message can be NULL
#define xassert_errno(condition, message)                                      \
  xassert_errno_((condition), (message), __FILE__, __LINE__);

void fatal(const char *format, ...);

// TODO: save work on failed assertion
void xassert_(bool condition, char *message, char *file, size_t line);
void xassert_errno_(bool condition, char *message, char *file, size_t line);
