#pragma once
#include <term/mode.h>

#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

// the implementation is in the header so that xmalloc can be inlined without
// LTO (Link Time Optimization)

// TODO: xassert
// TODO: rename to x.h
// TODO: make sure vec uses xmalloc/assert

// exits on failed allocation
// TODO: save work on failed allocation
static void* xmalloc(size_t size) {
  void* ptr = malloc(size);
  assert(ptr != NULL);
  return ptr;
}
