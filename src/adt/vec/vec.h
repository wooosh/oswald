#pragma once

// heavily based off of github.com/rxi/vec

#include <stddef.h>

#define vec_of(T)\
  struct { T *data; size_t length, capacity; }

typedef vec_of(char) vec_char;
typedef vec_of(const char) vec_const_char;

#define vec_unpack_(v)\
  (char**)&(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data)

#define vec_unpacked_sig_(v)\
  char** v ## _data,\
  size_t * v ## _length,\ 
  size_t * v ## _capacity,\
  size_t v ## _elem_size

void vec_init_(vec_unpacked_sig_(v)); 
#define vec_init(v)\
  vec_init_(vec_unpack_(v))

void vec_destroy_(vec_unpacked_sig_(v));
#define vec_destroy(v)\
  vec_destroy_(vec_unpack_(v)) 
