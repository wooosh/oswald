/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#include "vec.h"

#include <stddef.h>

size_t vec_expand_(char **data, size_t *length, size_t *capacity, size_t memsz) {
  if (*length + 1 > *capacity) {
    void *ptr;
    size_t n = (*capacity == 0) ? 1 : *capacity << 1;
    ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *data = ptr;
    *capacity = n;
  }
  return 0;
}


size_t vec_reserve_(char **data, size_t *length, size_t *capacity, size_t memsz, size_t n) {
  (void) length;
  if (n > *capacity) {
    void *ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *data = ptr;
    *capacity = n;
  }
  return 0;
}


size_t vec_reserve_po2_(
  char **data, size_t *length, size_t *capacity, size_t memsz, size_t n
) {
  size_t n2 = 1;
  if (n == 0) return 0;
  while (n2 < n) n2 <<= 1;
  return vec_reserve_(data, length, capacity, memsz, n2);
}


size_t vec_compact_(char **data, size_t *length, size_t *capacity, size_t memsz) {
  if (*length == 0) {
    free(*data);
    *data = NULL;
    *capacity = 0;
    return 0;
  } else {
    void *ptr;
    size_t n = *length;
    ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *capacity = n;
    *data = ptr;
  }
  return 0;
}


size_t vec_insert_(char **data, size_t *length, size_t *capacity, size_t memsz,
                 size_t idx
) {
  size_t err = vec_expand_(data, length, capacity, memsz);
  if (err) return err;
  memmove(*data + (idx + 1) * memsz,
          *data + idx * memsz,
          (*length - idx) * memsz);
  return 0;
}


void vec_splice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                 size_t start, size_t count
) {
  (void) capacity;
  memmove(*data + start * memsz,
          *data + (start + count) * memsz,
          (*length - start - count) * memsz);
}


void vec_swapsplice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                     size_t start, size_t count
) {
  (void) capacity;
  memmove(*data + start * memsz,
          *data + (*length - count) * memsz,
          count * memsz);
}


void vec_swap_(char **data, size_t *length, size_t *capacity, size_t memsz,
               size_t idx1, size_t idx2 
) {
  unsigned char *a, *b, tmp;
  size_t count;
  (void) length;
  (void) capacity;
  if (idx1 == idx2) return;
  a = (unsigned char*) *data + idx1 * memsz;
  b = (unsigned char*) *data + idx2 * memsz;
  count = memsz;
  while (count--) {
    tmp = *a;
    *a = *b;
    *b = tmp;
    a++, b++;
  }
}

