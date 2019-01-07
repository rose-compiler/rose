
// In this test case, my_tpl<int>::value is not qualified in the unparsed code

template<typename _Tp>
struct my_tpl {
  enum { value = 1 };
};

static_assert(my_tpl<int>::value, "my_tpl<int>::value == 0");

