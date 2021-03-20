#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <adt/vec.h>
#include <x.h>

void vec_init_(struct vec_generic_ v) {
  *v.data = NULL;
  *v.len = 0;
  *v.cap = 0;
}

void vec_destroy_(struct vec_generic_ v) {
  free(*v.data);
  vec_init_(v);
}

void vec_atleast_(struct vec_generic_ v, size_t cap) {
  if (*v.cap < cap) {
    if (*v.cap == 0) {
      *v.cap = 1;
    }

    while (*v.cap < cap)
      *v.cap *= 2;

    *v.data = xrealloc(*v.data, *v.cap * v.elem_size);
  }
}

void vec_splice_(struct vec_generic_ v, size_t idx, size_t len) {
  memmove(*v.data + idx * v.elem_size, *v.data + (idx + len) * v.elem_size,
          (*v.len - idx - len) * v.elem_size);
  *v.len -= len;
}

void vec_insert_gap_(struct vec_generic_ v, size_t idx, size_t len) {
  vec_atleast_(v, *v.len + len);
  memmove(*v.data + (idx + len) * v.elem_size, *v.data + idx * v.elem_size,
          (*v.len - idx) * v.elem_size);
  *v.len += len;
}

void vec_insert_vec_(struct vec_generic_ dest, struct vec_generic_ src,
                     size_t idx) {
  vec_insert_gap_(dest, idx, *src.len);
  memcpy(*dest.data + idx * dest.elem_size, *src.data,
         *src.len * dest.elem_size);
}

vec_const_char vec_from_str(char *str) {
  return (vec_const_char){str, strlen(str), strlen(str)};
}

void vec_printf(vec_char *v, const char* format, ...) {
  va_list args;
  va_start(args, format);
 
  // using vsnprintf destroys the first list, so we need to store a copy for
  // the second call 
  va_list args2;
  va_copy(args2, args);

  int len = vsnprintf(NULL, 0, format, args);
  // we need +1 because vsnprintf will include a null
  vec_atleast_(vec_repack_(v), v->len + len + 1);

  vsnprintf(v->data + v->len, len+1, format, args2);
  v->len += len;
}
