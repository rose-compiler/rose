// g0024.cc                               
// excerpt from gcc-2 header "stl_alloc.h" that contains invalid code
// but I need to allow if I want to handle code preprocessed with
// gcc-2

template <bool __threads, int __inst>
class __default_alloc_template {
  typedef int _Obj;
  static _Obj * volatile _S_free_list[16];
};

template <bool __threads, int __inst>
__default_alloc_template<__threads, __inst>::_Obj* volatile
__default_alloc_template<__threads, __inst> ::_S_free_list[
    16
] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };
