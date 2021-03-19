#include <string.h>

#include <adt/vec.h>

void vec_init_(vec_unpacked_sig_(v)) {
  *v_data = NULL;
  *v_length = 0;
  *v_capacity = 0;
}

void vec_destroy_(vec_unpacked_sig(v)) {
  free(v_data);
  vec_init_(v);
}
