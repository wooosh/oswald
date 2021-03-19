#define type_equal__(T1, T2) _Generic(( (T1){0} ), \
  T2: 1, \
  default: 0 \
)

// we need to compare types both ways because of how arrays decay to pointers
#define type_equal_(T1, T2)\
  (type_equal__(T1,T2) && type_equal__(T2,T1))

#define assert_type_equal(T1, T2, message)\
  _Static_assert(type_equal_(T1, T2), message)
