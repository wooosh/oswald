#pragma once

#include <stddef.h>
#include <stdbool.h>

// exits on failed allocation
void* xmalloc(size_t size);

// message must be provided
#define xassert(condition, message)\
  xassert_((condition), (message), __FILE__, __LINE__);

// message can be NULL
#define xassert_errno(condition, message)\
  xassert_errno_((condition), (message), __FILE__, __LINE__);

// TODO: save work on failed assertion
void xassert_(bool condition, char* message, char* file, size_t line);
void xassert_errno_(bool condition, char* message, char* file, size_t line);
