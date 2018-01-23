// It appears that char16_t is defined internally to EDG.
// typedef unsigned short char16_t;

  template<typename _Tp, _Tp __v> struct integral_constant {};

#if 1
  typedef integral_constant<bool, true>     true_type;
  typedef integral_constant<bool, false>    false_type;
#endif

  template<typename> struct __is_integral_helper : public false_type { };

  template<> struct __is_integral_helper<char16_t> : public true_type { };

#if 1
  template<> struct __is_integral_helper<short> : public true_type { };
#endif

#if 1
  template<> struct __is_integral_helper<unsigned short> : public true_type { };
#endif

