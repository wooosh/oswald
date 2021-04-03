#pragma once

// heavily based off of github.com/rxi/vec

// TODO: bounds checking
// TODO: rough type assertions using sizeof
// TODO: note that value must contain a constant expression and may be used 
// multiple times

#include <stddef.h>

#define vec_of(T)                                                              \
  struct {                                                                     \
    T *data;                                                                   \
    size_t len, cap;                                                           \
  }

typedef vec_of(int) vec_int;
typedef vec_of(char) vec_char;
typedef vec_of(const char) vec_const_char;

struct vec_generic_ {
  char **data;
  size_t *len;
  size_t *cap;
  size_t elem_size;
};

#define vec_repack_(v)                                                         \
  (struct vec_generic_) {                                                      \
    (char **)&(v)->data, &(v)->len, &(v)->cap, sizeof(*(v)->data)              \
  }

vec_const_char vec_from_str(char *str);

void vec_init_(struct vec_generic_ v);
#define vec_init(v) vec_init_(vec_repack_(v))

void vec_destroy_(struct vec_generic_ v);
#define vec_destroy(v) vec_destroy_(vec_repack_(v))

void vec_splice_(struct vec_generic_ v, size_t start, size_t end);
#define vec_splice(v, start, end) vec_splice_(vec_repack_(v), (start), (end))

// memory management functions
void vec_atleast_(struct vec_generic_ v, size_t cap);
void vec_insert_gap_(struct vec_generic_ v, size_t idx, size_t len);

#define vec_push(v, elem)                                                      \
  do {                                                                         \
    vec_atleast_(vec_repack_(v), (v)->len + 1);                                \
    (v)->data[(v)->len] = (elem);                                              \
    (v)->len++;                                                                \
  } while (0)

#define vec_truncate(v, len_)                                                  \
  do {                                                                         \
    (v)->len = (len_);                                                         \
  } while (0)

// TODO: cache idx and len everywhere
#define vec_set(v, value, idx, _len)                                            \
  for (size_t i__ = 0; i__ <= (_len); i__++)                                \
    (v)->data[i__ + (idx)] = (value);

#define vec_fill(v, value, idx, len)                                           \
  do {                                                                         \
    size_t idx__ = (idx);\
    size_t len__ = (len);\
    vec_insert_gap_(vec_repack_(v), idx__, len__);                             \
    vec_set(v, value, idx__, len__);                                               \
  } while (0)

void vec_insert_vec_(struct vec_generic_ dest, struct vec_generic_ src,
                     size_t idx);
#define vec_insert_vec(dest, src, idx)                                         \
  vec_insert_vec_(vec_repack_(dest), vec_repack_(src), (idx));

// TODO: should not be macro, should be function taking vec_char
#define vec_insert_str(dst, src, idx)                                          \
  do {                                                                         \
    vec_const_char v__ = vec_from_str(src);                                    \
    vec_insert_vec(dst, &v__, idx);                                            \
  } while (0)

#define vec_append_vec(dst, src) vec_insert_vec(dst, src, (dst)->len);

// TODO: should not be macro, should be function taking vec_char
// TODO|CLEANUP: append instead of insert to avoid memmove
#define vec_append_str(dst, src) vec_insert_str(dst, src, (dst)->len)

// appends printf'd string to end
void vec_printf(vec_char *v, const char* format, ...);
