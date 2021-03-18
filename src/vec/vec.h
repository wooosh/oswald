/** 
 * Copyright (c) 2014 rxi
 * Copyright (c) 2021 wooosh
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */

#ifndef VEC_H
#define VEC_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// TODO: note that these functions should not take functions calls as arguments in docs
// TODO: bounds checking on all functions
// TODO: convert everything to functions with vec_unpack, so that we don't have
// to worry about multiple use of arguments
// TODO: remove T from typenames
// TODO: can we make static asserts to make type errors cleaner?

#define vec_unpack_(v)\
  (char**)&(v)->data, &(v)->length, &(v)->capacity, sizeof(*(v)->data)


#define vec_t(T)\
  struct { T *data; size_t length, capacity; }


#define vec_const_t(T)\
  struct { const T *data; const size_t length, capacity; }


#define vec_init(v)\
  memset((v), 0, sizeof(*(v)))


#define vec_deinit(v)\
  ( free((v)->data),\
    vec_init(v) ) 


#define vec_push(v, val)\
  ( vec_expand_(vec_unpack_(v)) ? -1 :\
    ((v)->data[(v)->length++] = (val), 0), 0 )


#define vec_pop(v)\
  (v)->data[--(v)->length]


#define vec_splice(v, start, count)\
  ( vec_splice_(vec_unpack_(v), start, count),\
    (v)->length -= (count) )


#define vec_swapsplice(v, start, count)\
  ( vec_swapsplice_(vec_unpack_(v), start, count),\
    (v)->length -= (count) )


#define vec_insert(v, idx, val)\
  ( vec_insert_(vec_unpack_(v), idx) ? -1 :\
    ((v)->data[idx] = (val), 0), (v)->length++, 0 )
    
// TODO|CLEANUP: optimize this to use memmove and memcpy in vec.c
#define vec_insert_vec(v1, idx, v2)\
  for (size_t i__= 0; i__<(v2)->length; i__++)\
    vec_insert((v1), (idx) + i__, (v2)->data[i__]);

#define vec_sort(v, fn)\
  qsort((v)->data, (v)->length, sizeof(*(v)->data), fn)


#define vec_swap(v, idx1, idx2)\
  vec_swap_(vec_unpack_(v), idx1, idx2)


#define vec_truncate(v, len)\
  ((v)->length = (len) < (v)->length ? (len) : (v)->length)


#define vec_clear(v)\
  ((v)->length = 0)


#define vec_first(v)\
  (v)->data[0]


#define vec_last(v)\
  (v)->data[(v)->length - 1]


#define vec_reserve(v, n)\
  vec_reserve_(vec_unpack_(v), n)

 
#define vec_compact(v)\
  vec_compact_(vec_unpack_(v))


#define vec_pusharr(v, arr, count)\
  do {\
    size_t i__, n__ = (count);\
    if (vec_reserve_po2_(vec_unpack_(v), (v)->length + n__) != 0) break;\
    for (i__ = 0; i__ < n__; i__++) {\
      (v)->data[(v)->length++] = (arr)[i__];\
    }\
  } while (0)

#define vec_pushstr(v, str)\
  vec_pusharr((v), (str), strlen((str)))

#define vec_extend(v, v2)\
  vec_pusharr((v), (v2)->data, (v2)->length)


#define vec_find(v, val, idx)\
  do {\
    for ((idx) = 0; (idx) < (v)->length; (idx)++) {\
      if ((v)->data[(idx)] == (val)) break;\
    }\
    if ((idx) == (v)->length) (idx) = -1;\
  } while (0)


#define vec_remove(v, val)\
  do {\
    size_t idx__;\
    vec_find(v, val, idx__);\
    if (idx__ != -1) vec_splice(v, idx__, 1);\
  } while (0)


#define vec_reverse(v)\
  do {\
    size_t i__ = (v)->length / 2;\
    while (i__--) {\
      vec_swap((v), i__, (v)->length - (i__ + 1));\
    }\
  } while (0)


#define vec_foreach(v, var, iter)\
  if  ( (v)->length > 0 )\
  for ( (iter) = 0;\
        (iter) < (v)->length && (((var) = (v)->data[(iter)]), 1);\
        ++(iter))


#define vec_foreach_rev(v, var, iter)\
  if  ( (v)->length > 0 )\
  for ( (iter) = (v)->length - 1;\
        (iter) >= 0 && (((var) = (v)->data[(iter)]), 1);\
        --(iter))


#define vec_foreach_ptr(v, var, iter)\
  if  ( (v)->length > 0 )\
  for ( (iter) = 0;\
        (iter) < (v)->length && (((var) = &(v)->data[(iter)]), 1);\
        ++(iter))


#define vec_foreach_ptr_rev(v, var, iter)\
  if  ( (v)->length > 0 )\
  for ( (iter) = (v)->length - 1;\
        (iter) >= 0 && (((var) = &(v)->data[(iter)]), 1);\
        --(iter))

// memset for vectors
#define vec_set(v, value, start, end)\
  for (size_t i__ =(start); i__<(end); i__++)\
    (v)->data[i__] = value;

// TODO: should take a starting index, need to memmove instead of reserve
#define vec_fill(v, value, len)\
  do {\
    vec_reserve((v), (len));\
    (v)->length = (len);\
    vec_set((v), 0, (len), (value));\
  } while (0)


size_t vec_expand_(char **data, size_t *length, size_t *capacity, size_t memsz);
size_t vec_reserve_(char **data, size_t *length, size_t *capacity, size_t memsz, size_t n);
size_t vec_reserve_po2_(char **data, size_t *length, size_t *capacity, size_t memsz,
                     size_t n);
size_t vec_compact_(char **data, size_t *length, size_t *capacity, size_t memsz);
size_t vec_insert_(char **data, size_t *length, size_t *capacity, size_t memsz,
                size_t idx);
void vec_splice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                 size_t start, size_t count);
void vec_swapsplice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                     size_t start, size_t count);
void vec_swap_(char **data, size_t *length, size_t *capacity, size_t memsz,
               size_t idx1, size_t idx2);


typedef vec_t(void*) vec_void_t;
typedef vec_t(char*) vec_str_t;
typedef vec_t(int) vec_int_t;
typedef vec_t(char) vec_char_t;
typedef vec_const_t(char) vec_const_char_t;
typedef vec_t(float) vec_float_t;
typedef vec_t(double) vec_double_t;

#endif
