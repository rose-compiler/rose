  struct __true_type { };
  struct __false_type { };

  template<bool>
    struct __truth_type
    { typedef __false_type __type; };

#if 1
  template<>
    struct __truth_type<true>
    { typedef __true_type __type; };
#endif
