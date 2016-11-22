 
long long          __builtin_bswap64(long long __X);
void               __builtin_ia32_fxsave(void *__P);
void               __builtin_ia32_fxrstor(void *__P);
void               __builtin_ia32_fxsave64(void *__P);
void               __builtin_ia32_fxrstor64(void *__P);

 
namespace std
{
  typedef long unsigned int 	size_t;
  typedef long int	ptrdiff_t;

  typedef decltype(nullptr)	nullptr_t;
}

namespace std
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}
namespace __gnu_cxx
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}

namespace std 
{
  void
  __throw_bad_exception(void) __attribute__((__noreturn__));

  void
  __throw_bad_alloc(void) __attribute__((__noreturn__));

  
  void
  __throw_bad_cast(void) __attribute__((__noreturn__));

  void
  __throw_bad_typeid(void) __attribute__((__noreturn__));

  
  void
  __throw_logic_error(const char*) __attribute__((__noreturn__));

  void
  __throw_domain_error(const char*) __attribute__((__noreturn__));

  void
  __throw_invalid_argument(const char*) __attribute__((__noreturn__));

  void
  __throw_length_error(const char*) __attribute__((__noreturn__));

  void
  __throw_out_of_range(const char*) __attribute__((__noreturn__));

  void
  __throw_out_of_range_fmt(const char*, ...) __attribute__((__noreturn__))
    __attribute__((__format__(__gnu_printf__, 1, 2)));

  void
  __throw_runtime_error(const char*) __attribute__((__noreturn__));

  void
  __throw_range_error(const char*) __attribute__((__noreturn__));

  void
  __throw_overflow_error(const char*) __attribute__((__noreturn__));

  void
  __throw_underflow_error(const char*) __attribute__((__noreturn__));

  
  void
  __throw_ios_failure(const char*) __attribute__((__noreturn__));

  void
  __throw_system_error(int) __attribute__((__noreturn__));

  void
  __throw_future_error(int) __attribute__((__noreturn__));

  
  void
  __throw_bad_function_call() __attribute__((__noreturn__));

} 



extern "C++" {

namespace std 
{


  struct __true_type { };
  struct __false_type { };

  template<bool>
    struct __truth_type
    { typedef __false_type __type; };

  template<>
    struct __truth_type<true>
    { typedef __true_type __type; };

  
  
  template<class _Sp, class _Tp>
    struct __traitor
    {
      enum { __value = bool(_Sp::__value) || bool(_Tp::__value) };
      typedef typename __truth_type<__value>::__type __type;
    };

  
  template<typename, typename>
    struct __are_same
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Tp>
    struct __are_same<_Tp, _Tp>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  template<typename _Tp>
    struct __is_void
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_void<void>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_integer
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  
  
  
  
  template<>
    struct __is_integer<bool>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<signed char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<wchar_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char16_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<char32_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<short>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned short>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<int>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned int>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<long long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_integer<unsigned long long>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };


template< > struct __is_integer<__int128> { enum { __value = 1 }; typedef __true_type __type; }; template< > struct __is_integer<unsigned __int128> { enum { __value = 1 }; typedef __true_type __type; };


  
  
  
  template<typename _Tp>
    struct __is_floating
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  
  template<>
    struct __is_floating<float>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_floating<double>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_floating<long double>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_pointer
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<typename _Tp>
    struct __is_pointer<_Tp*>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_arithmetic
    : public __traitor<__is_integer<_Tp>, __is_floating<_Tp> >
    { };

  
  
  
  template<typename _Tp>
    struct __is_scalar
    : public __traitor<__is_arithmetic<_Tp>, __is_pointer<_Tp> >
    { };

  
  
  
  template<typename _Tp>
    struct __is_char
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_char<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_char<wchar_t>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<typename _Tp>
    struct __is_byte
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  template<>
    struct __is_byte<char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_byte<signed char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<>
    struct __is_byte<unsigned char>
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  
  
  
  template<typename _Tp>
    struct __is_move_iterator
    {
      enum { __value = 0 };
      typedef __false_type __type;
    };

  
  
  template<typename _Iterator>
    inline _Iterator
    __miter_base(_Iterator __it)
    { return __it; }


} 
} 


extern "C++" {

namespace __gnu_cxx 
{


  
  template<bool, typename>
    struct __enable_if 
    { };

  template<typename _Tp>
    struct __enable_if<true, _Tp>
    { typedef _Tp __type; };


  
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    struct __conditional_type
    { typedef _Iftrue __type; };

  template<typename _Iftrue, typename _Iffalse>
    struct __conditional_type<false, _Iftrue, _Iffalse>
    { typedef _Iffalse __type; };


  
  template<typename _Tp>
    struct __add_unsigned
    { 
    private:
      typedef __enable_if<std::__is_integer<_Tp>::__value, _Tp> __if_type;
      
    public:
      typedef typename __if_type::__type __type; 
    };

  template<>
    struct __add_unsigned<char>
    { typedef unsigned char __type; };

  template<>
    struct __add_unsigned<signed char>
    { typedef unsigned char __type; };

  template<>
    struct __add_unsigned<short>
    { typedef unsigned short __type; };

  template<>
    struct __add_unsigned<int>
    { typedef unsigned int __type; };

  template<>
    struct __add_unsigned<long>
    { typedef unsigned long __type; };

  template<>
    struct __add_unsigned<long long>
    { typedef unsigned long long __type; };

  
  template<>
    struct __add_unsigned<bool>;

  template<>
    struct __add_unsigned<wchar_t>;


  
  template<typename _Tp>
    struct __remove_unsigned
    { 
    private:
      typedef __enable_if<std::__is_integer<_Tp>::__value, _Tp> __if_type;
      
    public:
      typedef typename __if_type::__type __type; 
    };

  template<>
    struct __remove_unsigned<char>
    { typedef signed char __type; };

  template<>
    struct __remove_unsigned<unsigned char>
    { typedef signed char __type; };

  template<>
    struct __remove_unsigned<unsigned short>
    { typedef short __type; };

  template<>
    struct __remove_unsigned<unsigned int>
    { typedef int __type; };

  template<>
    struct __remove_unsigned<unsigned long>
    { typedef long __type; };

  template<>
    struct __remove_unsigned<unsigned long long>
    { typedef long long __type; };

  
  template<>
    struct __remove_unsigned<bool>;

  template<>
    struct __remove_unsigned<wchar_t>;


  
  template<typename _Type>
    inline bool
    __is_null_pointer(_Type* __ptr)
    { return __ptr == 0; }

  template<typename _Type>
    inline bool
    __is_null_pointer(_Type)
    { return false; }

  inline bool
  __is_null_pointer(std::nullptr_t)
  { return true; }

  
  template<typename _Tp, bool = std::__is_integer<_Tp>::__value>
    struct __promote
    { typedef double __type; };

  
  
  
  template<typename _Tp>
    struct __promote<_Tp, false>
    { };

  template<>
    struct __promote<long double>
    { typedef long double __type; };

  template<>
    struct __promote<double>
    { typedef double __type; };

  template<>
    struct __promote<float>
    { typedef float __type; };

  template<typename _Tp, typename _Up,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type>
    struct __promote_2
    {
      typedef __typeof__(_Tp2() + _Up2()) __type;
    };

  template<typename _Tp, typename _Up, typename _Vp,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type,
           typename _Vp2 = typename __promote<_Vp>::__type>
    struct __promote_3
    {
      typedef __typeof__(_Tp2() + _Up2() + _Vp2()) __type;
    };

  template<typename _Tp, typename _Up, typename _Vp, typename _Wp,
           typename _Tp2 = typename __promote<_Tp>::__type,
           typename _Up2 = typename __promote<_Up>::__type,
           typename _Vp2 = typename __promote<_Vp>::__type,
           typename _Wp2 = typename __promote<_Wp>::__type>
    struct __promote_4
    {
      typedef __typeof__(_Tp2() + _Up2() + _Vp2() + _Wp2()) __type;
    };


} 
} 


namespace __gnu_cxx 
{

  template<typename _Value>
    struct __numeric_traits_integer
    {
      
      static const _Value __min = (((_Value)(-1) < 0) ? (_Value)1 << (sizeof(_Value) * 8 - ((_Value)(-1) < 0)) : (_Value)0);
      static const _Value __max = (((_Value)(-1) < 0) ? (((((_Value)1 << ((sizeof(_Value) * 8 - ((_Value)(-1) < 0)) - 1)) - 1) << 1) + 1) : ~(_Value)0);

      
      
      static const bool __is_signed = ((_Value)(-1) < 0);
      static const int __digits = (sizeof(_Value) * 8 - ((_Value)(-1) < 0));      
    };

  template<typename _Value>
    const _Value __numeric_traits_integer<_Value>::__min;

  template<typename _Value>
    const _Value __numeric_traits_integer<_Value>::__max;

  template<typename _Value>
    const bool __numeric_traits_integer<_Value>::__is_signed;

  template<typename _Value>
    const int __numeric_traits_integer<_Value>::__digits;






  template<typename _Value>
    struct __numeric_traits_floating
    {
      
      static const int __max_digits10 = (2 + (std::__are_same<_Value, float> ::__value ? 24 : std::__are_same<_Value, double> ::__value ? 53 : 64) * 643L / 2136);

      
      static const bool __is_signed = true;
      static const int __digits10 = (std::__are_same<_Value, float> ::__value ? 6 : std::__are_same<_Value, double> ::__value ? 15 : 18);
      static const int __max_exponent10 = (std::__are_same<_Value, float> ::__value ? 38 : std::__are_same<_Value, double> ::__value ? 308 : 4932);
    };

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__max_digits10;

  template<typename _Value>
    const bool __numeric_traits_floating<_Value>::__is_signed;

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__digits10;

  template<typename _Value>
    const int __numeric_traits_floating<_Value>::__max_exponent10;

  template<typename _Value>
    struct __numeric_traits
    : public __conditional_type<std::__is_integer<_Value>::__value,
				__numeric_traits_integer<_Value>,
				__numeric_traits_floating<_Value> >::__type
    { };


} 


namespace std 
{

  template<typename _Tp>
    inline _Tp*
    __addressof(_Tp& __r) noexcept
    {
      return reinterpret_cast<_Tp*>
	(&const_cast<char&>(reinterpret_cast<const volatile char&>(__r)));
    }


} 

namespace std
{
  typedef short unsigned int uint_least16_t;
  typedef unsigned int uint_least32_t;
}

namespace std 
{
  
  template<typename _Tp, _Tp __v>
    struct integral_constant
    {
      static constexpr _Tp                  value = __v;
      typedef _Tp                           value_type;
      typedef integral_constant<_Tp, __v>   type;
      constexpr operator value_type() const { return value; }


      constexpr value_type operator()() const { return value; }
    };
  
  template<typename _Tp, _Tp __v>
    constexpr _Tp integral_constant<_Tp, __v>::value;

  
  typedef integral_constant<bool, true>     true_type;

  
  typedef integral_constant<bool, false>    false_type;

  template<bool __v>
    using __bool_constant = integral_constant<bool, __v>;

  template<bool, typename, typename>
    struct conditional;

  template<typename...>
    struct __or_;

  template<>
    struct __or_<>
    : public false_type
    { };

  template<typename _B1>
    struct __or_<_B1>
    : public _B1
    { };

  template<typename _B1, typename _B2>
    struct __or_<_B1, _B2>
    : public conditional<_B1::value, _B1, _B2>::type
    { };

  template<typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __or_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, _B1, __or_<_B2, _B3, _Bn...>>::type
    { };

  template<typename...>
    struct __and_;

  template<>
    struct __and_<>
    : public true_type
    { };

  template<typename _B1>
    struct __and_<_B1>
    : public _B1
    { };

  template<typename _B1, typename _B2>
    struct __and_<_B1, _B2>
    : public conditional<_B1::value, _B2, _B1>::type
    { };

  template<typename _B1, typename _B2, typename _B3, typename... _Bn>
    struct __and_<_B1, _B2, _B3, _Bn...>
    : public conditional<_B1::value, __and_<_B2, _B3, _Bn...>, _B1>::type
    { };

  template<typename _Pp>
    struct __not_
    : public integral_constant<bool, !_Pp::value>
    { };


  
  
  
  

  template<typename _Tp>
    struct __success_type
    { typedef _Tp type; };

  struct __failure_type
  { };

  

  template<typename>
    struct remove_cv;

  template<typename>
    struct __is_void_helper
    : public false_type { };

  template<>
    struct __is_void_helper<void>
    : public true_type { };

  
  template<typename _Tp>
    struct is_void
    : public __is_void_helper<typename remove_cv<_Tp>::type>::type
    { };

  template<typename>
    struct __is_integral_helper
    : public false_type { };

  template<>
    struct __is_integral_helper<bool>
    : public true_type { };
  
  template<>
    struct __is_integral_helper<char>
    : public true_type { };

  template<>
    struct __is_integral_helper<signed char>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned char>
    : public true_type { };

  template<>
    struct __is_integral_helper<wchar_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<char16_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<char32_t>
    : public true_type { };

  template<>
    struct __is_integral_helper<short>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned short>
    : public true_type { };

  template<>
    struct __is_integral_helper<int>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned int>
    : public true_type { };

  template<>
    struct __is_integral_helper<long>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned long>
    : public true_type { };

  template<>
    struct __is_integral_helper<long long>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned long long>
    : public true_type { };

  
  
  template<>
    struct __is_integral_helper<__int128>
    : public true_type { };

  template<>
    struct __is_integral_helper<unsigned __int128>
    : public true_type { };

  
  template<typename _Tp>
    struct is_integral
    : public __is_integral_helper<typename remove_cv<_Tp>::type>::type
    { };

  template<typename>
    struct __is_floating_point_helper
    : public false_type { };

  template<>
    struct __is_floating_point_helper<float>
    : public true_type { };

  template<>
    struct __is_floating_point_helper<double>
    : public true_type { };

  template<>
    struct __is_floating_point_helper<long double>
    : public true_type { };


  
  template<typename _Tp>
    struct is_floating_point
    : public __is_floating_point_helper<typename remove_cv<_Tp>::type>::type
    { };

  
  template<typename>
    struct is_array
    : public false_type { };

  template<typename _Tp, std::size_t _Size>
    struct is_array<_Tp[_Size]>
    : public true_type { };

  template<typename _Tp>
    struct is_array<_Tp[]>
    : public true_type { };

  template<typename>
    struct __is_pointer_helper
    : public false_type { };

  template<typename _Tp>
    struct __is_pointer_helper<_Tp*>
    : public true_type { };

  
  template<typename _Tp>
    struct is_pointer
    : public __is_pointer_helper<typename remove_cv<_Tp>::type>::type
    { };

  
  template<typename>
    struct is_lvalue_reference
    : public false_type { };

  template<typename _Tp>
    struct is_lvalue_reference<_Tp&>
    : public true_type { };

  
  template<typename>
    struct is_rvalue_reference
    : public false_type { };

  template<typename _Tp>
    struct is_rvalue_reference<_Tp&&>
    : public true_type { };

  template<typename>
    struct is_function;

  template<typename>
    struct __is_member_object_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_object_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, !is_function<_Tp>::value> { };

  
  template<typename _Tp>
    struct is_member_object_pointer
    : public __is_member_object_pointer_helper<
				typename remove_cv<_Tp>::type>::type
    { };

  template<typename>
    struct __is_member_function_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_function_pointer_helper<_Tp _Cp::*>
    : public integral_constant<bool, is_function<_Tp>::value> { };

  
  template<typename _Tp>
    struct is_member_function_pointer
    : public __is_member_function_pointer_helper<
				typename remove_cv<_Tp>::type>::type
    { };

  
  template<typename _Tp>
    struct is_enum
    : public integral_constant<bool, __is_enum(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_union
    : public integral_constant<bool, __is_union(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_class
    : public integral_constant<bool, __is_class(_Tp)>
    { };

  
  template<typename>
    struct is_function
    : public false_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...)>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......)>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) volatile &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) volatile &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) volatile &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) volatile &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const volatile &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes...) const volatile &&>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const volatile>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const volatile &>
    : public true_type { };

  template<typename _Res, typename... _ArgTypes>
    struct is_function<_Res(_ArgTypes......) const volatile &&>
    : public true_type { };


  template<typename>
    struct __is_null_pointer_helper
    : public false_type { };

  template<>
    struct __is_null_pointer_helper<std::nullptr_t>
    : public true_type { };

  
  template<typename _Tp>
    struct is_null_pointer
    : public __is_null_pointer_helper<typename remove_cv<_Tp>::type>::type
    { };

  
  template<typename _Tp>
    struct __is_nullptr_t
    : public is_null_pointer<_Tp>
    { };

  

  
  template<typename _Tp>
    struct is_reference
    : public __or_<is_lvalue_reference<_Tp>,
                   is_rvalue_reference<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_arithmetic
    : public __or_<is_integral<_Tp>, is_floating_point<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_fundamental
    : public __or_<is_arithmetic<_Tp>, is_void<_Tp>,
		   is_null_pointer<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_object
    : public __not_<__or_<is_function<_Tp>, is_reference<_Tp>,
                          is_void<_Tp>>>::type
    { };

  template<typename>
    struct is_member_pointer;

  
  template<typename _Tp>
    struct is_scalar
    : public __or_<is_arithmetic<_Tp>, is_enum<_Tp>, is_pointer<_Tp>,
                   is_member_pointer<_Tp>, is_null_pointer<_Tp>>::type
    { };

  
  template<typename _Tp>
    struct is_compound
    : public integral_constant<bool, !is_fundamental<_Tp>::value> { };

  template<typename _Tp>
    struct __is_member_pointer_helper
    : public false_type { };

  template<typename _Tp, typename _Cp>
    struct __is_member_pointer_helper<_Tp _Cp::*>
    : public true_type { };

  
  template<typename _Tp>
    struct is_member_pointer
    : public __is_member_pointer_helper<typename remove_cv<_Tp>::type>::type
    { };

  

  template<typename _Tp>
    struct __is_referenceable
    : public __or_<is_object<_Tp>, is_reference<_Tp>>::type
    { };

  template<typename _Res, typename... _Args>
    struct __is_referenceable<_Res(_Args...)>
    : public true_type
    { };

  template<typename _Res, typename... _Args>
    struct __is_referenceable<_Res(_Args......)>
    : public true_type
    { };

  

  
  template<typename>
    struct is_const
    : public false_type { };

  template<typename _Tp>
    struct is_const<_Tp const>
    : public true_type { };
  
  
  template<typename>
    struct is_volatile
    : public false_type { };

  template<typename _Tp>
    struct is_volatile<_Tp volatile>
    : public true_type { };

  
  template<typename _Tp>
    struct is_trivial
    : public integral_constant<bool, __is_trivial(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_trivially_copyable
    : public integral_constant<bool, __is_trivially_copyable(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_standard_layout
    : public integral_constant<bool, __is_standard_layout(_Tp)>
    { };

  
  
  template<typename _Tp>
    struct is_pod
    : public integral_constant<bool, __is_pod(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_literal_type
    : public integral_constant<bool, __is_literal_type(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_empty
    : public integral_constant<bool, __is_empty(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_polymorphic
    : public integral_constant<bool, __is_polymorphic(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_final
    : public integral_constant<bool, __is_final(_Tp)>
    { };

  
  template<typename _Tp>
    struct is_abstract
    : public integral_constant<bool, __is_abstract(_Tp)>
    { };

  template<typename _Tp,
	   bool = is_arithmetic<_Tp>::value>
    struct __is_signed_helper
    : public false_type { };

  template<typename _Tp>
    struct __is_signed_helper<_Tp, true>
    : public integral_constant<bool, _Tp(-1) < _Tp(0)>
    { };

  
  template<typename _Tp>
    struct is_signed
    : public __is_signed_helper<_Tp>::type
    { };

  
  template<typename _Tp>
    struct is_unsigned
    : public __and_<is_arithmetic<_Tp>, __not_<is_signed<_Tp>>>
    { };


  

  template<typename>
    struct add_rvalue_reference;

  


 
  template<typename _Tp>
    typename add_rvalue_reference<_Tp>::type declval() noexcept;

  template<typename, unsigned = 0>
    struct extent;

  template<typename>
    struct remove_all_extents;

  template<typename _Tp>
    struct __is_array_known_bounds
    : public integral_constant<bool, (extent<_Tp>::value > 0)>
    { };

  template<typename _Tp>
    struct __is_array_unknown_bounds
    : public __and_<is_array<_Tp>, __not_<extent<_Tp>>>
    { };
    
  
  
  
  
  
  struct __do_is_destructible_impl
  {
    template<typename _Tp, typename = decltype(declval<_Tp&>().~_Tp())>
      static true_type __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_destructible_impl
    : public __do_is_destructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp,
           bool = __or_<is_void<_Tp>,
                        __is_array_unknown_bounds<_Tp>,
                        is_function<_Tp>>::value,
           bool = __or_<is_reference<_Tp>, is_scalar<_Tp>>::value>
    struct __is_destructible_safe;

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, false, false>
    : public __is_destructible_impl<typename
               remove_all_extents<_Tp>::type>::type
    { };

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, true, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_destructible_safe<_Tp, false, true>
    : public true_type { };

  
  template<typename _Tp>
    struct is_destructible
    : public __is_destructible_safe<_Tp>::type
    { };

  
  
  
  
  struct __do_is_nt_destructible_impl
  {
    template<typename _Tp>
      static integral_constant<bool, noexcept(declval<_Tp&>().~_Tp())>
        __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_nt_destructible_impl
    : public __do_is_nt_destructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp,
           bool = __or_<is_void<_Tp>,
                        __is_array_unknown_bounds<_Tp>,
                        is_function<_Tp>>::value,
           bool = __or_<is_reference<_Tp>, is_scalar<_Tp>>::value>
    struct __is_nt_destructible_safe;

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, false, false>
    : public __is_nt_destructible_impl<typename
               remove_all_extents<_Tp>::type>::type
    { };

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, true, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_destructible_safe<_Tp, false, true>
    : public true_type { };

  
  template<typename _Tp>
    struct is_nothrow_destructible
    : public __is_nt_destructible_safe<_Tp>::type
    { };

  struct __do_is_default_constructible_impl
  {
    template<typename _Tp, typename = decltype(_Tp())>
      static true_type __test(int);

    template<typename>
      static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_default_constructible_impl
    : public __do_is_default_constructible_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp>
    struct __is_default_constructible_atom
    : public __and_<__not_<is_void<_Tp>>,
                    __is_default_constructible_impl<_Tp>>
    { };

  template<typename _Tp, bool = is_array<_Tp>::value>
    struct __is_default_constructible_safe;

  
  
  
  
  
  template<typename _Tp>
    struct __is_default_constructible_safe<_Tp, true>
    : public __and_<__is_array_known_bounds<_Tp>,
		    __is_default_constructible_atom<typename
                      remove_all_extents<_Tp>::type>>
    { };

  template<typename _Tp>
    struct __is_default_constructible_safe<_Tp, false>
    : public __is_default_constructible_atom<_Tp>::type
    { };

  
  template<typename _Tp>
    struct is_default_constructible
    : public __is_default_constructible_safe<_Tp>::type
    { };


  

  
  
  
  
  
  
  
  
  
  struct __do_is_static_castable_impl
  {
    template<typename _From, typename _To, typename
             = decltype(static_cast<_To>(declval<_From>()))>
      static true_type __test(int);

    template<typename, typename>
      static false_type __test(...);
  };

  template<typename _From, typename _To>
    struct __is_static_castable_impl
    : public __do_is_static_castable_impl
    {
      typedef decltype(__test<_From, _To>(0)) type;
    };

  template<typename _From, typename _To>
    struct __is_static_castable_safe
    : public __is_static_castable_impl<_From, _To>::type
    { };

  
  template<typename _From, typename _To>
    struct __is_static_castable
    : public integral_constant<bool, (__is_static_castable_safe<
				      _From, _To>::value)>
    { };

  
  
  
  
  
  struct __do_is_direct_constructible_impl
  {
    template<typename _Tp, typename _Arg, typename
	     = decltype(::new _Tp(declval<_Arg>()))>
      static true_type __test(int);

    template<typename, typename>
      static false_type __test(...);
  };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_impl
    : public __do_is_direct_constructible_impl
    {
      typedef decltype(__test<_Tp, _Arg>(0)) type;
    };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_new_safe
    : public __and_<is_destructible<_Tp>,
                    __is_direct_constructible_impl<_Tp, _Arg>>
    { };

  template<typename, typename>
    struct is_same;

  template<typename, typename>
    struct is_base_of;

  template<typename>
    struct remove_reference;

  template<typename _From, typename _To, bool
           = __not_<__or_<is_void<_From>, 
                          is_function<_From>>>::value>
    struct __is_base_to_derived_ref;

  
  
  template<typename _From, typename _To>
    struct __is_base_to_derived_ref<_From, _To, true>
    {
      typedef typename remove_cv<typename remove_reference<_From
        >::type>::type __src_t;
      typedef typename remove_cv<typename remove_reference<_To
        >::type>::type __dst_t;
      typedef __and_<__not_<is_same<__src_t, __dst_t>>,
		     is_base_of<__src_t, __dst_t>> type;
      static constexpr bool value = type::value;
    };

  template<typename _From, typename _To>
    struct __is_base_to_derived_ref<_From, _To, false>
    : public false_type
    { };

  template<typename _From, typename _To, bool
           = __and_<is_lvalue_reference<_From>,
                    is_rvalue_reference<_To>>::value>
    struct __is_lvalue_to_rvalue_ref;

  
  
  template<typename _From, typename _To>
    struct __is_lvalue_to_rvalue_ref<_From, _To, true>
    {
      typedef typename remove_cv<typename remove_reference<
        _From>::type>::type __src_t;
      typedef typename remove_cv<typename remove_reference<
        _To>::type>::type __dst_t;
      typedef __and_<__not_<is_function<__src_t>>, 
        __or_<is_same<__src_t, __dst_t>,
		    is_base_of<__dst_t, __src_t>>> type;
      static constexpr bool value = type::value;
    };

  template<typename _From, typename _To>
    struct __is_lvalue_to_rvalue_ref<_From, _To, false>
    : public false_type
    { };

  
  
  
  
  
  
  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_ref_cast
    : public __and_<__is_static_castable<_Arg, _Tp>,
                    __not_<__or_<__is_base_to_derived_ref<_Arg, _Tp>,
                                 __is_lvalue_to_rvalue_ref<_Arg, _Tp>
                   >>>
    { };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible_new
    : public conditional<is_reference<_Tp>::value,
			 __is_direct_constructible_ref_cast<_Tp, _Arg>,
			 __is_direct_constructible_new_safe<_Tp, _Arg>
			 >::type
    { };

  template<typename _Tp, typename _Arg>
    struct __is_direct_constructible
    : public __is_direct_constructible_new<_Tp, _Arg>::type
    { };

  
  
  
  
  
  struct __do_is_nary_constructible_impl
  {
    template<typename _Tp, typename... _Args, typename
             = decltype(_Tp(declval<_Args>()...))>
      static true_type __test(int);

    template<typename, typename...>
      static false_type __test(...);
  };

  template<typename _Tp, typename... _Args>
    struct __is_nary_constructible_impl
    : public __do_is_nary_constructible_impl
    {
      typedef decltype(__test<_Tp, _Args...>(0)) type;
    };

  template<typename _Tp, typename... _Args>
    struct __is_nary_constructible
    : public __is_nary_constructible_impl<_Tp, _Args...>::type
    {
      static_assert(sizeof...(_Args) > 1,
                    "Only useful for > 1 arguments");
    };

  template<typename _Tp, typename... _Args>
    struct __is_constructible_impl
    : public __is_nary_constructible<_Tp, _Args...>
    { };

  template<typename _Tp, typename _Arg>
    struct __is_constructible_impl<_Tp, _Arg>
    : public __is_direct_constructible<_Tp, _Arg>
    { };

  template<typename _Tp>
    struct __is_constructible_impl<_Tp>
    : public is_default_constructible<_Tp>
    { };

  
  template<typename _Tp, typename... _Args>
    struct is_constructible
    : public __is_constructible_impl<_Tp, _Args...>::type
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_copy_constructible_impl;

  template<typename _Tp>
    struct __is_copy_constructible_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_copy_constructible_impl<_Tp, true>
    : public is_constructible<_Tp, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_copy_constructible
    : public __is_copy_constructible_impl<_Tp>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_move_constructible_impl;

  template<typename _Tp>
    struct __is_move_constructible_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_move_constructible_impl<_Tp, true>
    : public is_constructible<_Tp, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_move_constructible
    : public __is_move_constructible_impl<_Tp>
    { };

  template<typename _Tp>
    struct __is_nt_default_constructible_atom
    : public integral_constant<bool, noexcept(_Tp())>
    { };

  template<typename _Tp, bool = is_array<_Tp>::value>
    struct __is_nt_default_constructible_impl;

  template<typename _Tp>
    struct __is_nt_default_constructible_impl<_Tp, true>
    : public __and_<__is_array_known_bounds<_Tp>,
		    __is_nt_default_constructible_atom<typename
                      remove_all_extents<_Tp>::type>>
    { };

  template<typename _Tp>
    struct __is_nt_default_constructible_impl<_Tp, false>
    : public __is_nt_default_constructible_atom<_Tp>
    { };

  
  template<typename _Tp>
    struct is_nothrow_default_constructible
    : public __and_<is_default_constructible<_Tp>,
                    __is_nt_default_constructible_impl<_Tp>>
    { };

  template<typename _Tp, typename... _Args>
    struct __is_nt_constructible_impl
    : public integral_constant<bool, noexcept(_Tp(declval<_Args>()...))>
    { };

  template<typename _Tp, typename _Arg>
    struct __is_nt_constructible_impl<_Tp, _Arg>
    : public integral_constant<bool,
                               noexcept(static_cast<_Tp>(declval<_Arg>()))>
    { };

  template<typename _Tp>
    struct __is_nt_constructible_impl<_Tp>
    : public is_nothrow_default_constructible<_Tp>
    { };

  
  template<typename _Tp, typename... _Args>
    struct is_nothrow_constructible
    : public __and_<is_constructible<_Tp, _Args...>,
		    __is_nt_constructible_impl<_Tp, _Args...>>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_nothrow_copy_constructible_impl;

  template<typename _Tp>
    struct __is_nothrow_copy_constructible_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nothrow_copy_constructible_impl<_Tp, true>
    : public is_nothrow_constructible<_Tp, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_copy_constructible
    : public __is_nothrow_copy_constructible_impl<_Tp>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_nothrow_move_constructible_impl;

  template<typename _Tp>
    struct __is_nothrow_move_constructible_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nothrow_move_constructible_impl<_Tp, true>
    : public is_nothrow_constructible<_Tp, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_move_constructible
    : public __is_nothrow_move_constructible_impl<_Tp>
    { };

  template<typename _Tp, typename _Up>
    class __is_assignable_helper
    {
      template<typename _Tp1, typename _Up1,
	       typename = decltype(declval<_Tp1>() = declval<_Up1>())>
	static true_type
	__test(int);

      template<typename, typename>
	static false_type
	__test(...);

    public:
      typedef decltype(__test<_Tp, _Up>(0)) type;
    };

  
  template<typename _Tp, typename _Up>
    struct is_assignable
      : public __is_assignable_helper<_Tp, _Up>::type
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_copy_assignable_impl;

  template<typename _Tp>
    struct __is_copy_assignable_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_copy_assignable_impl<_Tp, true>
    : public is_assignable<_Tp&, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_copy_assignable
    : public __is_copy_assignable_impl<_Tp>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_move_assignable_impl;

  template<typename _Tp>
    struct __is_move_assignable_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_move_assignable_impl<_Tp, true>
    : public is_assignable<_Tp&, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_move_assignable
    : public __is_move_assignable_impl<_Tp>
    { };

  template<typename _Tp, typename _Up>
    struct __is_nt_assignable_impl
    : public integral_constant<bool, noexcept(declval<_Tp>() = declval<_Up>())>
    { };

  
  template<typename _Tp, typename _Up>
    struct is_nothrow_assignable
    : public __and_<is_assignable<_Tp, _Up>,
		    __is_nt_assignable_impl<_Tp, _Up>>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_nt_copy_assignable_impl;

  template<typename _Tp>
    struct __is_nt_copy_assignable_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_copy_assignable_impl<_Tp, true>
    : public is_nothrow_assignable<_Tp&, const _Tp&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_copy_assignable
    : public __is_nt_copy_assignable_impl<_Tp>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __is_nt_move_assignable_impl;

  template<typename _Tp>
    struct __is_nt_move_assignable_impl<_Tp, false>
    : public false_type { };

  template<typename _Tp>
    struct __is_nt_move_assignable_impl<_Tp, true>
    : public is_nothrow_assignable<_Tp&, _Tp&&>
    { };

  
  template<typename _Tp>
    struct is_nothrow_move_assignable
    : public __is_nt_move_assignable_impl<_Tp>
    { };

  
  template<typename _Tp, typename... _Args>
    struct is_trivially_constructible
    : public __and_<is_constructible<_Tp, _Args...>, integral_constant<bool,
			__is_trivially_constructible(_Tp, _Args...)>>
    { };
  
  
  template<typename _Tp>
    struct is_trivially_default_constructible
    : public is_trivially_constructible<_Tp>::type
    { };

  struct __do_is_implicitly_default_constructible_impl
  {
    template <typename _Tp>
    static void __helper(const _Tp&);

    template <typename _Tp>
    static true_type __test(const _Tp&,
                            decltype(__helper<const _Tp&>({}))* = 0);

    static false_type __test(...);
  };

  template<typename _Tp>
    struct __is_implicitly_default_constructible_impl
      : public __do_is_implicitly_default_constructible_impl
  {
    typedef decltype(__test(declval<_Tp>())) type;
  };

  template<typename _Tp>
    struct __is_implicitly_default_constructible_safe
      : public __is_implicitly_default_constructible_impl<_Tp>::type
  { };

  template <typename _Tp>
    struct __is_implicitly_default_constructible
      : public __and_<is_default_constructible<_Tp>,
                      __is_implicitly_default_constructible_safe<_Tp>>
  { };

  
  template<typename _Tp>
    struct is_trivially_copy_constructible
    : public __and_<is_copy_constructible<_Tp>, 
		    integral_constant<bool,
			__is_trivially_constructible(_Tp, const _Tp&)>>
    { };
  
  
  template<typename _Tp>
    struct is_trivially_move_constructible
    : public __and_<is_move_constructible<_Tp>, 
		    integral_constant<bool,
			__is_trivially_constructible(_Tp, _Tp&&)>>
    { };

  
  template<typename _Tp, typename _Up>
    struct is_trivially_assignable
    : public __and_<is_assignable<_Tp, _Up>, 
		    integral_constant<bool,
			__is_trivially_assignable(_Tp, _Up)>>
    { };

  
  template<typename _Tp>
    struct is_trivially_copy_assignable
    : public __and_<is_copy_assignable<_Tp>, 
		    integral_constant<bool,
			__is_trivially_assignable(_Tp&, const _Tp&)>>
    { };

  
  template<typename _Tp>
    struct is_trivially_move_assignable
    : public __and_<is_move_assignable<_Tp>, 
		    integral_constant<bool,
			__is_trivially_assignable(_Tp&, _Tp&&)>>
    { };

  
  template<typename _Tp>
    struct is_trivially_destructible
    : public __and_<is_destructible<_Tp>, integral_constant<bool,
			      __has_trivial_destructor(_Tp)>>
    { };

  
  template<typename _Tp>
    struct has_trivial_default_constructor
    : public integral_constant<bool, __has_trivial_constructor(_Tp)>
    { } __attribute__ ((__deprecated__));

  
  template<typename _Tp>
    struct has_trivial_copy_constructor
    : public integral_constant<bool, __has_trivial_copy(_Tp)>
    { } __attribute__ ((__deprecated__));

  
  template<typename _Tp>
    struct has_trivial_copy_assign
    : public integral_constant<bool, __has_trivial_assign(_Tp)>
    { } __attribute__ ((__deprecated__));

  
  template<typename _Tp>
    struct has_virtual_destructor
    : public integral_constant<bool, __has_virtual_destructor(_Tp)>
    { };

  
  

  
  template<typename _Tp>
    struct alignment_of
    : public integral_constant<std::size_t, __alignof__(_Tp)> { };
  
  
  template<typename>
    struct rank
    : public integral_constant<std::size_t, 0> { };
   
  template<typename _Tp, std::size_t _Size>
    struct rank<_Tp[_Size]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  template<typename _Tp>
    struct rank<_Tp[]>
    : public integral_constant<std::size_t, 1 + rank<_Tp>::value> { };

  
  template<typename, unsigned _Uint>
    struct extent
    : public integral_constant<std::size_t, 0> { };
  
  template<typename _Tp, unsigned _Uint, std::size_t _Size>
    struct extent<_Tp[_Size], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? _Size : extent<_Tp,
							   _Uint - 1>::value>
    { };

  template<typename _Tp, unsigned _Uint>
    struct extent<_Tp[], _Uint>
    : public integral_constant<std::size_t,
			       _Uint == 0 ? 0 : extent<_Tp,
						       _Uint - 1>::value>
    { };


  

  
  template<typename, typename>
    struct is_same
    : public false_type { };

  template<typename _Tp>
    struct is_same<_Tp, _Tp>
    : public true_type { };

  
  template<typename _Base, typename _Derived>
    struct is_base_of
    : public integral_constant<bool, __is_base_of(_Base, _Derived)>
    { };

  template<typename _From, typename _To,
           bool = __or_<is_void<_From>, is_function<_To>,
                        is_array<_To>>::value>
    struct __is_convertible_helper
    { typedef typename is_void<_To>::type type; };

  template<typename _From, typename _To>
    class __is_convertible_helper<_From, _To, false>
    {
       template<typename _To1>
	static void __test_aux(_To1);

      template<typename _From1, typename _To1,
	       typename = decltype(__test_aux<_To1>(std::declval<_From1>()))>
	static true_type
	__test(int);

      template<typename, typename>
	static false_type
	__test(...);

    public:
      typedef decltype(__test<_From, _To>(0)) type;
    };


  
  template<typename _From, typename _To>
    struct is_convertible
    : public __is_convertible_helper<_From, _To>::type
    { };


  

  
  template<typename _Tp>
    struct remove_const
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_const<_Tp const>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_volatile
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_volatile<_Tp volatile>
    { typedef _Tp     type; };
  
  
  template<typename _Tp>
    struct remove_cv
    {
      typedef typename
      remove_const<typename remove_volatile<_Tp>::type>::type     type;
    };
  
  
  template<typename _Tp>
    struct add_const
    { typedef _Tp const     type; };
   
  
  template<typename _Tp>
    struct add_volatile
    { typedef _Tp volatile     type; };
  
  
  template<typename _Tp>
    struct add_cv
    {
      typedef typename
      add_const<typename add_volatile<_Tp>::type>::type     type;
    };



  
  template<typename _Tp>
    using remove_const_t = typename remove_const<_Tp>::type;

  
  template<typename _Tp>
    using remove_volatile_t = typename remove_volatile<_Tp>::type;

  
  template<typename _Tp>
    using remove_cv_t = typename remove_cv<_Tp>::type;

  
  template<typename _Tp>
    using add_const_t = typename add_const<_Tp>::type;

  
  template<typename _Tp>
    using add_volatile_t = typename add_volatile<_Tp>::type;

  
  template<typename _Tp>
    using add_cv_t = typename add_cv<_Tp>::type;

  

  
  template<typename _Tp>
    struct remove_reference
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&>
    { typedef _Tp   type; };

  template<typename _Tp>
    struct remove_reference<_Tp&&>
    { typedef _Tp   type; };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __add_lvalue_reference_helper
    { typedef _Tp   type; };

  template<typename _Tp>
    struct __add_lvalue_reference_helper<_Tp, true>
    { typedef _Tp&   type; };

  
  template<typename _Tp>
    struct add_lvalue_reference
    : public __add_lvalue_reference_helper<_Tp>
    { };

  template<typename _Tp, bool = __is_referenceable<_Tp>::value>
    struct __add_rvalue_reference_helper
    { typedef _Tp   type; };

  template<typename _Tp>
    struct __add_rvalue_reference_helper<_Tp, true>
    { typedef _Tp&&   type; };

  
  template<typename _Tp>
    struct add_rvalue_reference
    : public __add_rvalue_reference_helper<_Tp>
    { };

  
  template<typename _Tp>
    using remove_reference_t = typename remove_reference<_Tp>::type;

  
  template<typename _Tp>
    using add_lvalue_reference_t = typename add_lvalue_reference<_Tp>::type;

  
  template<typename _Tp>
    using add_rvalue_reference_t = typename add_rvalue_reference<_Tp>::type;

  

  
  template<typename _Unqualified, bool _IsConst, bool _IsVol>
    struct __cv_selector;

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, false, false>
    { typedef _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, false, true>
    { typedef volatile _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, true, false>
    { typedef const _Unqualified __type; };

  template<typename _Unqualified>
    struct __cv_selector<_Unqualified, true, true>
    { typedef const volatile _Unqualified __type; };

  template<typename _Qualified, typename _Unqualified,
	   bool _IsConst = is_const<_Qualified>::value,
	   bool _IsVol = is_volatile<_Qualified>::value>
    class __match_cv_qualifiers
    {
      typedef __cv_selector<_Unqualified, _IsConst, _IsVol> __match;

    public:
      typedef typename __match::__type __type; 
    };

  
  template<typename _Tp>
    struct __make_unsigned
    { typedef _Tp __type; };

  template<>
    struct __make_unsigned<char>
    { typedef unsigned char __type; };

  template<>
    struct __make_unsigned<signed char>
    { typedef unsigned char __type; };

  template<>
    struct __make_unsigned<short>
    { typedef unsigned short __type; };

  template<>
    struct __make_unsigned<int>
    { typedef unsigned int __type; };

  template<>
    struct __make_unsigned<long>
    { typedef unsigned long __type; };

  template<>
    struct __make_unsigned<long long>
    { typedef unsigned long long __type; };

  template<>
    struct __make_unsigned<wchar_t> : __make_unsigned<int>
    { };

  template<>
    struct __make_unsigned<__int128>
    { typedef unsigned __int128 __type; };

  
  template<typename _Tp, 
	   bool _IsInt = is_integral<_Tp>::value,
	   bool _IsEnum = is_enum<_Tp>::value>
    class __make_unsigned_selector;

  template<typename _Tp>
    class __make_unsigned_selector<_Tp, true, false>
    {
      typedef __make_unsigned<typename remove_cv<_Tp>::type> __unsignedt;
      typedef typename __unsignedt::__type __unsigned_type;
      typedef __match_cv_qualifiers<_Tp, __unsigned_type> __cv_unsigned;

    public:
      typedef typename __cv_unsigned::__type __type;
    };

  template<typename _Tp>
    class __make_unsigned_selector<_Tp, false, true>
    {
      
      typedef unsigned char __smallest;
      static const bool __b0 = sizeof(_Tp) <= sizeof(__smallest);
      static const bool __b1 = sizeof(_Tp) <= sizeof(unsigned short);
      static const bool __b2 = sizeof(_Tp) <= sizeof(unsigned int);
      static const bool __b3 = sizeof(_Tp) <= sizeof(unsigned long);
      typedef conditional<__b3, unsigned long, unsigned long long> __cond3;
      typedef typename __cond3::type __cond3_type;
      typedef conditional<__b2, unsigned int, __cond3_type> __cond2;
      typedef typename __cond2::type __cond2_type;
      typedef conditional<__b1, unsigned short, __cond2_type> __cond1;
      typedef typename __cond1::type __cond1_type;

      typedef typename conditional<__b0, __smallest, __cond1_type>::type
	__unsigned_type;
      typedef __match_cv_qualifiers<_Tp, __unsigned_type> __cv_unsigned;

    public:
      typedef typename __cv_unsigned::__type __type;
    };

  
  
  
  
  template<typename _Tp>
    struct make_unsigned 
    { typedef typename __make_unsigned_selector<_Tp>::__type type; };

  
  template<>
    struct make_unsigned<bool>;


  
  template<typename _Tp>
    struct __make_signed
    { typedef _Tp __type; };

  template<>
    struct __make_signed<char>
    { typedef signed char __type; };

  template<>
    struct __make_signed<unsigned char>
    { typedef signed char __type; };

  template<>
    struct __make_signed<unsigned short>
    { typedef signed short __type; };

  template<>
    struct __make_signed<unsigned int>
    { typedef signed int __type; };

  template<>
    struct __make_signed<unsigned long>
    { typedef signed long __type; };

  template<>
    struct __make_signed<unsigned long long>
    { typedef signed long long __type; };


  template<>
    struct __make_signed<char16_t> : __make_signed<uint_least16_t>
    { };
  template<>
    struct __make_signed<char32_t> : __make_signed<uint_least32_t>
    { };

  template<>
    struct __make_signed<unsigned __int128>
    { typedef __int128 __type; };

  
  template<typename _Tp, 
	   bool _IsInt = is_integral<_Tp>::value,
	   bool _IsEnum = is_enum<_Tp>::value>
    class __make_signed_selector;

  template<typename _Tp>
    class __make_signed_selector<_Tp, true, false>
    {
      typedef __make_signed<typename remove_cv<_Tp>::type> __signedt;
      typedef typename __signedt::__type __signed_type;
      typedef __match_cv_qualifiers<_Tp, __signed_type> __cv_signed;

    public:
      typedef typename __cv_signed::__type __type;
    };

  template<typename _Tp>
    class __make_signed_selector<_Tp, false, true>
    {
      typedef typename __make_unsigned_selector<_Tp>::__type __unsigned_type;

    public:
      typedef typename __make_signed_selector<__unsigned_type>::__type __type;
    };

  
  
  
  
  template<typename _Tp>
    struct make_signed 
    { typedef typename __make_signed_selector<_Tp>::__type type; };

  
  template<>
    struct make_signed<bool>;

  
  template<typename _Tp>
    using make_signed_t = typename make_signed<_Tp>::type;

  
  template<typename _Tp>
    using make_unsigned_t = typename make_unsigned<_Tp>::type;

  

  
  template<typename _Tp>
    struct remove_extent
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_extent<_Tp[_Size]>
    { typedef _Tp     type; };

  template<typename _Tp>
    struct remove_extent<_Tp[]>
    { typedef _Tp     type; };

  
  template<typename _Tp>
    struct remove_all_extents
    { typedef _Tp     type; };

  template<typename _Tp, std::size_t _Size>
    struct remove_all_extents<_Tp[_Size]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

  template<typename _Tp>
    struct remove_all_extents<_Tp[]>
    { typedef typename remove_all_extents<_Tp>::type     type; };

  
  template<typename _Tp>
    using remove_extent_t = typename remove_extent<_Tp>::type;

  
  template<typename _Tp>
    using remove_all_extents_t = typename remove_all_extents<_Tp>::type;

  

  template<typename _Tp, typename>
    struct __remove_pointer_helper
    { typedef _Tp     type; };

  template<typename _Tp, typename _Up>
    struct __remove_pointer_helper<_Tp, _Up*>
    { typedef _Up     type; };

  
  template<typename _Tp>
    struct remove_pointer
    : public __remove_pointer_helper<_Tp, typename remove_cv<_Tp>::type>
    { };

  
  template<typename _Tp, bool = __or_<__is_referenceable<_Tp>,
				      is_void<_Tp>>::value>
    struct __add_pointer_helper
    { typedef _Tp     type; };

  template<typename _Tp>
    struct __add_pointer_helper<_Tp, true>
    { typedef typename remove_reference<_Tp>::type*     type; };

  template<typename _Tp>
    struct add_pointer 
    : public __add_pointer_helper<_Tp>
    { };

  
  template<typename _Tp>
    using remove_pointer_t = typename remove_pointer<_Tp>::type;

  
  template<typename _Tp>
    using add_pointer_t = typename add_pointer<_Tp>::type;

  template<std::size_t _Len>
    struct __aligned_storage_msa
    { 
      union __type
      {
	unsigned char __data[_Len];
	struct __attribute__((__aligned__)) { } __align; 
      };
    };

  








 
  template<std::size_t _Len, std::size_t _Align =
	   __alignof__(typename __aligned_storage_msa<_Len>::__type)>
    struct aligned_storage
    { 
      union type
      {
	unsigned char __data[_Len];
	struct __attribute__((__aligned__((_Align)))) { } __align; 
      };
    };

  template <typename... _Types>
    struct __strictest_alignment
    {
      static const size_t _S_alignment = 0;
      static const size_t _S_size = 0;
    };

  template <typename _Tp, typename... _Types>
    struct __strictest_alignment<_Tp, _Types...>
    {
      static const size_t _S_alignment =
        alignof(_Tp) > __strictest_alignment<_Types...>::_S_alignment
	? alignof(_Tp) : __strictest_alignment<_Types...>::_S_alignment;
      static const size_t _S_size =
        sizeof(_Tp) > __strictest_alignment<_Types...>::_S_size
	? sizeof(_Tp) : __strictest_alignment<_Types...>::_S_size;
    };

  








 
  template <size_t _Len, typename... _Types>
    struct aligned_union
    {
    private:
      static_assert(sizeof...(_Types) != 0, "At least one type is required");

      using __strictest = __strictest_alignment<_Types...>;
      static const size_t _S_len = _Len > __strictest::_S_size
	? _Len : __strictest::_S_size;
    public:
      
      static const size_t alignment_value = __strictest::_S_alignment;
      
      typedef typename aligned_storage<_S_len, alignment_value>::type type;
    };

  template <size_t _Len, typename... _Types>
    const size_t aligned_union<_Len, _Types...>::alignment_value;

  
  
  template<typename _Up, 
	   bool _IsArray = is_array<_Up>::value,
	   bool _IsFunction = is_function<_Up>::value> 
    struct __decay_selector;

  
  template<typename _Up> 
    struct __decay_selector<_Up, false, false>
    { typedef typename remove_cv<_Up>::type __type; };

  template<typename _Up> 
    struct __decay_selector<_Up, true, false>
    { typedef typename remove_extent<_Up>::type* __type; };

  template<typename _Up> 
    struct __decay_selector<_Up, false, true>
    { typedef typename add_pointer<_Up>::type __type; };

  
  template<typename _Tp> 
    class decay 
    { 
      typedef typename remove_reference<_Tp>::type __remove_type;

    public:
      typedef typename __decay_selector<__remove_type>::__type type;
    };

  template<typename _Tp>
    class reference_wrapper;

  
  template<typename _Tp>
    struct __strip_reference_wrapper
    {
      typedef _Tp __type;
    };

  template<typename _Tp>
    struct __strip_reference_wrapper<reference_wrapper<_Tp> >
    {
      typedef _Tp& __type;
    };

  template<typename _Tp>
    struct __decay_and_strip
    {
      typedef typename __strip_reference_wrapper<
	typename decay<_Tp>::type>::__type __type;
    };


  
  
  template<bool, typename _Tp = void>
    struct enable_if 
    { };

  
  template<typename _Tp>
    struct enable_if<true, _Tp>
    { typedef _Tp type; };

  template<typename... _Cond>
    using _Require = typename enable_if<__and_<_Cond...>::value>::type;

  
  
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    struct conditional
    { typedef _Iftrue type; };

  
  template<typename _Iftrue, typename _Iffalse>
    struct conditional<false, _Iftrue, _Iffalse>
    { typedef _Iffalse type; };

  
  template<typename... _Tp>
    struct common_type;

  

  struct __do_common_type_impl
  {
    template<typename _Tp, typename _Up>
      static __success_type<typename decay<decltype
			    (true ? std::declval<_Tp>()
			     : std::declval<_Up>())>::type> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _Tp, typename _Up>
    struct __common_type_impl
    : private __do_common_type_impl
    {
      typedef decltype(_S_test<_Tp, _Up>(0)) type;
    };

  struct __do_member_type_wrapper
  {
    template<typename _Tp>
      static __success_type<typename _Tp::type> _S_test(int);

    template<typename>
      static __failure_type _S_test(...);
  };

  template<typename _Tp>
    struct __member_type_wrapper
    : private __do_member_type_wrapper
    {
      typedef decltype(_S_test<_Tp>(0)) type;
    };

  template<typename _CTp, typename... _Args>
    struct __expanded_common_type_wrapper
    {
      typedef common_type<typename _CTp::type, _Args...> type;
    };

  template<typename... _Args>
    struct __expanded_common_type_wrapper<__failure_type, _Args...>
    { typedef __failure_type type; };

  template<typename _Tp>
    struct common_type<_Tp>
    { typedef typename decay<_Tp>::type type; };

  template<typename _Tp, typename _Up>
    struct common_type<_Tp, _Up>
    : public __common_type_impl<_Tp, _Up>::type
    { };

  template<typename _Tp, typename _Up, typename... _Vp>
    struct common_type<_Tp, _Up, _Vp...>
    : public __expanded_common_type_wrapper<typename __member_type_wrapper<
               common_type<_Tp, _Up>>::type, _Vp...>::type
    { };

  
  template<typename _Tp>
    struct underlying_type
    {
      typedef __underlying_type(_Tp) type;
    };

  template<typename _Tp>
    struct __declval_protector
    {
      static const bool __stop = false;
      static typename add_rvalue_reference<_Tp>::type __delegate();
    };

  template<typename _Tp>
    inline typename add_rvalue_reference<_Tp>::type
    declval() noexcept
    {
      static_assert(__declval_protector<_Tp>::__stop,
		    "declval() must not be used!");
      return __declval_protector<_Tp>::__delegate();
    }

  
  template<typename _Signature>
    class result_of;

  


  struct __invoke_memfun_ref { };
  struct __invoke_memfun_deref { };
  struct __invoke_memobj_ref { };
  struct __invoke_memobj_deref { };
  struct __invoke_other { };

  
  template<typename _Tp, typename _Tag>
    struct __result_of_success : __success_type<_Tp>
    { using __invoke_type = _Tag; };

  
  struct __result_of_memfun_ref_impl
  {
    template<typename _Fp, typename _Tp1, typename... _Args>
      static __result_of_success<decltype(
      (std::declval<_Tp1>().*std::declval<_Fp>())(std::declval<_Args>()...)
      ), __invoke_memfun_ref> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun_ref
    : private __result_of_memfun_ref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
    };

  
  struct __result_of_memfun_deref_impl
  {
    template<typename _Fp, typename _Tp1, typename... _Args>
      static __result_of_success<decltype(
      ((*std::declval<_Tp1>()).*std::declval<_Fp>())(std::declval<_Args>()...)
      ), __invoke_memfun_deref> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun_deref
    : private __result_of_memfun_deref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg, _Args...>(0)) type;
    };

  
  struct __result_of_memobj_ref_impl
  {
    template<typename _Fp, typename _Tp1>
      static __result_of_success<decltype(
      std::declval<_Tp1>().*std::declval<_Fp>()
      ), __invoke_memobj_ref> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj_ref
    : private __result_of_memobj_ref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
    };

  
  struct __result_of_memobj_deref_impl
  {
    template<typename _Fp, typename _Tp1>
      static __result_of_success<decltype(
      (*std::declval<_Tp1>()).*std::declval<_Fp>()
      ), __invoke_memobj_deref> _S_test(int);

    template<typename, typename>
      static __failure_type _S_test(...);
  };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj_deref
    : private __result_of_memobj_deref_impl
    {
      typedef decltype(_S_test<_MemPtr, _Arg>(0)) type;
    };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_memobj;

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, _Arg>
    {
      typedef typename remove_cv<typename remove_reference<
        _Arg>::type>::type _Argval;
      typedef _Res _Class::* _MemPtr;
      typedef typename conditional<__or_<is_same<_Argval, _Class>,
        is_base_of<_Class, _Argval>>::value,
        __result_of_memobj_ref<_MemPtr, _Arg>,
        __result_of_memobj_deref<_MemPtr, _Arg>
      >::type::type type;
    };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_memfun;

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, _Arg, _Args...>
    {
      typedef typename remove_cv<typename remove_reference<
        _Arg>::type>::type _Argval;
      typedef _Res _Class::* _MemPtr;
      typedef typename conditional<__or_<is_same<_Argval, _Class>,
        is_base_of<_Class, _Argval>>::value,
        __result_of_memfun_ref<_MemPtr, _Arg, _Args...>,
        __result_of_memfun_deref<_MemPtr, _Arg, _Args...>
      >::type::type type;
    };

  
  
  

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, reference_wrapper<_Arg>>
    : __result_of_memobj_ref<_Res _Class::*, _Arg&>
    { };

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, reference_wrapper<_Arg>&>
    : __result_of_memobj_ref<_Res _Class::*, _Arg&>
    { };

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, const reference_wrapper<_Arg>&>
    : __result_of_memobj_ref<_Res _Class::*, _Arg&>
    { };

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, reference_wrapper<_Arg>&&>
    : __result_of_memobj_ref<_Res _Class::*, _Arg&>
    { };

  template<typename _Res, typename _Class, typename _Arg>
    struct __result_of_memobj<_Res _Class::*, const reference_wrapper<_Arg>&&>
    : __result_of_memobj_ref<_Res _Class::*, _Arg&>
    { };

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, reference_wrapper<_Arg>, _Args...>
    : __result_of_memfun_ref<_Res _Class::*, _Arg&, _Args...>
    { };

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, reference_wrapper<_Arg>&,
			      _Args...>
    : __result_of_memfun_ref<_Res _Class::*, _Arg&, _Args...>
    { };

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, const reference_wrapper<_Arg>&,
			      _Args...>
    : __result_of_memfun_ref<_Res _Class::*, _Arg&, _Args...>
    { };

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, reference_wrapper<_Arg>&&,
			      _Args...>
    : __result_of_memfun_ref<_Res _Class::*, _Arg&, _Args...>
    { };

  template<typename _Res, typename _Class, typename _Arg, typename... _Args>
    struct __result_of_memfun<_Res _Class::*, const reference_wrapper<_Arg>&&,
			      _Args...>
    : __result_of_memfun_ref<_Res _Class::*, _Arg&, _Args...>
    { };

  template<bool, bool, typename _Functor, typename... _ArgTypes>
    struct __result_of_impl
    {
      typedef __failure_type type;
    };

  template<typename _MemPtr, typename _Arg>
    struct __result_of_impl<true, false, _MemPtr, _Arg>
    : public __result_of_memobj<typename decay<_MemPtr>::type, _Arg>
    { };

  template<typename _MemPtr, typename _Arg, typename... _Args>
    struct __result_of_impl<false, true, _MemPtr, _Arg, _Args...>
    : public __result_of_memfun<typename decay<_MemPtr>::type, _Arg, _Args...>
    { };

  
  struct __result_of_other_impl
  {
    template<typename _Fn, typename... _Args>
      static __result_of_success<decltype(
      std::declval<_Fn>()(std::declval<_Args>()...)
      ), __invoke_other> _S_test(int);

    template<typename...>
      static __failure_type _S_test(...);
  };

  template<typename _Functor, typename... _ArgTypes>
    struct __result_of_impl<false, false, _Functor, _ArgTypes...>
    : private __result_of_other_impl
    {
      typedef decltype(_S_test<_Functor, _ArgTypes...>(0)) type;
    };

  template<typename _Functor, typename... _ArgTypes>
    struct result_of<_Functor(_ArgTypes...)>
    : public __result_of_impl<
        is_member_object_pointer<
          typename remove_reference<_Functor>::type
        >::value,
        is_member_function_pointer<
          typename remove_reference<_Functor>::type
        >::value,
	    _Functor, _ArgTypes...
      >::type
    { };

  
  template<size_t _Len, size_t _Align =
	    __alignof__(typename __aligned_storage_msa<_Len>::__type)>
    using aligned_storage_t = typename aligned_storage<_Len, _Align>::type;

  template <size_t _Len, typename... _Types>
    using aligned_union_t = typename aligned_union<_Len, _Types...>::type;

  
  template<typename _Tp>
    using decay_t = typename decay<_Tp>::type;

  
  template<bool _Cond, typename _Tp = void>
    using enable_if_t = typename enable_if<_Cond, _Tp>::type;

  
  template<bool _Cond, typename _Iftrue, typename _Iffalse>
    using conditional_t = typename conditional<_Cond, _Iftrue, _Iffalse>::type;

  
  template<typename... _Tp>
    using common_type_t = typename common_type<_Tp...>::type;

  
  template<typename _Tp>
    using underlying_type_t = typename underlying_type<_Tp>::type;

  
  template<typename _Tp>
    using result_of_t = typename result_of<_Tp>::type;

  template<typename...> using __void_t = void;

  
  template<typename...> using void_t = void;

  
  template<typename _Default, typename _AlwaysVoid,
	   template<typename...> class _Op, typename... _Args>
    struct __detector
    {
      using value_t = false_type;
      using type = _Default;
    };

  
  template<typename _Default, template<typename...> class _Op,
	    typename... _Args>
    struct __detector<_Default, __void_t<_Op<_Args...>>, _Op, _Args...>
    {
      using value_t = true_type;
      using type = _Op<_Args...>;
    };

  
  template<typename _Default, template<typename...> class _Op,
	   typename... _Args>
    using __detected_or = __detector<_Default, void, _Op, _Args...>;

  
  template<typename _Default, template<typename...> class _Op,
	   typename... _Args>
    using __detected_or_t
      = typename __detected_or<_Default, _Op, _Args...>::type;

  
  template<template<typename...> class _Default,
	   template<typename...> class _Op, typename... _Args>
    using __detected_or_t_ =
      __detected_or_t<_Default<_Args...>, _Op, _Args...>;

  

  


 

  template <typename _Tp>
    struct __is_swappable;

  template <typename _Tp>
    struct __is_nothrow_swappable;

  template<typename _Tp>
    inline
    typename enable_if<__and_<is_move_constructible<_Tp>,
			      is_move_assignable<_Tp>>::value>::type
    swap(_Tp&, _Tp&)
    noexcept(__and_<is_nothrow_move_constructible<_Tp>,
	            is_nothrow_move_assignable<_Tp>>::value);

  template<typename _Tp, size_t _Nm>
    inline
    typename enable_if<__is_swappable<_Tp>::value>::type
    swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm])
    noexcept(__is_nothrow_swappable<_Tp>::value);

  namespace __swappable_details {
    using std::swap;

    struct __do_is_swappable_impl
    {
      template<typename _Tp, typename
               = decltype(swap(std::declval<_Tp&>(), std::declval<_Tp&>()))>
        static true_type __test(int);

      template<typename>
        static false_type __test(...);
    };

    struct __do_is_nothrow_swappable_impl
    {
      template<typename _Tp>
        static __bool_constant<
          noexcept(swap(std::declval<_Tp&>(), std::declval<_Tp&>()))
        > __test(int);

      template<typename>
        static false_type __test(...);
    };

  }

  template<typename _Tp>
    struct __is_swappable_impl
    : public __swappable_details::__do_is_swappable_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp>
    struct __is_nothrow_swappable_impl
    : public __swappable_details::__do_is_nothrow_swappable_impl
    {
      typedef decltype(__test<_Tp>(0)) type;
    };

  template<typename _Tp>
    struct __is_swappable
    : public __is_swappable_impl<_Tp>::type
    { };

  template<typename _Tp>
    struct __is_nothrow_swappable
    : public __is_nothrow_swappable_impl<_Tp>::type
    { };


} 



namespace std 
{


  


 

  




 
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type& __t) noexcept
    { return static_cast<_Tp&&>(__t); }

  




 
  template<typename _Tp>
    constexpr _Tp&&
    forward(typename std::remove_reference<_Tp>::type&& __t) noexcept
    {
      static_assert(!std::is_lvalue_reference<_Tp>::value, "template argument"
		    " substituting _Tp is an lvalue reference type");
      return static_cast<_Tp&&>(__t);
    }

  



 
  template<typename _Tp>
    constexpr typename std::remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept
    { return static_cast<typename std::remove_reference<_Tp>::type&&>(__t); }


  template<typename _Tp>
    struct __move_if_noexcept_cond
    : public __and_<__not_<is_nothrow_move_constructible<_Tp>>,
                    is_copy_constructible<_Tp>>::type { };

  






 
  template<typename _Tp>
    constexpr typename
    conditional<__move_if_noexcept_cond<_Tp>::value, const _Tp&, _Tp&&>::type
    move_if_noexcept(_Tp& __x) noexcept
    { return std::move(__x); }

  

  





 
  template<typename _Tp>
    inline _Tp*
    addressof(_Tp& __r) noexcept
    { return std::__addressof(__r); }

  
  template <typename _Tp, typename _Up = _Tp>
    inline _Tp
    __exchange(_Tp& __obj, _Up&& __new_val)
    {
      _Tp __old_val = std::move(__obj);
      __obj = std::forward<_Up>(__new_val);
      return __old_val;
    }

  

} 


namespace std 
{


  


 

  




 
  template<typename _Tp>
    inline
    typename enable_if<__and_<is_move_constructible<_Tp>,
			      is_move_assignable<_Tp>>::value>::type
    swap(_Tp& __a, _Tp& __b)
    noexcept(__and_<is_nothrow_move_constructible<_Tp>,
	            is_nothrow_move_assignable<_Tp>>::value)
    {
      
      

      _Tp __tmp = std::move(__a);
      __a = std::move(__b);
      __b = std::move(__tmp);
    }

  
  
  
  template<typename _Tp, size_t _Nm>
    inline
    typename enable_if<__is_swappable<_Tp>::value>::type
    swap(_Tp (&__a)[_Nm], _Tp (&__b)[_Nm])
    noexcept(__is_nothrow_swappable<_Tp>::value)
    {
      for (size_t __n = 0; __n < _Nm; ++__n)
	swap(__a[__n], __b[__n]);
    }

  

} 



namespace std 
{


  


 

  
  struct piecewise_construct_t { explicit piecewise_construct_t() = default; };

  
  constexpr piecewise_construct_t piecewise_construct = piecewise_construct_t();

  
  template<typename...>
    class tuple;

  template<std::size_t...>
    struct _Index_tuple;

  
  
  
  
  
  template <typename _T1, typename _T2, typename _U1, typename _U2>
  constexpr bool _ConstructiblePair()
  {
    return __and_<__or_<is_same<typename decay<_T1>::type,
				typename decay<_U1>::type>,
			is_constructible<_T1, const _U1&>>,
		  __or_<is_same<typename decay<_T2>::type,
				typename decay<_U2>::type>,
			is_constructible<_T2, const _U2&>>>::value;
  }

  template <typename _T1, typename _T2, typename _U1, typename _U2>
  constexpr bool _ImplicitlyConvertiblePair()
  {
    return __and_<__or_<is_same<typename decay<_T1>::type,
				typename decay<_U1>::type>,
			is_convertible<const _U1&, _T1>>,
		  __or_<is_same<typename decay<_T2>::type,
				typename decay<_U2>::type>,
		       is_convertible<const _U2&, _T2>>>::value;
  }

  template <typename _T1, typename _T2, typename _U1, typename _U2>
  constexpr bool _MoveConstructiblePair()
  {
    return __and_<__or_<is_same<typename decay<_T1>::type,
				typename decay<_U1>::type>,
			is_constructible<_T1, _U1&&>>,
		  __or_<is_same<typename decay<_T2>::type,
				typename decay<_U2>::type>,
			is_constructible<_T2, _U2&&>>>::value;
  }

  template <typename _T1, typename _T2, typename _U1, typename _U2>
  constexpr bool _ImplicitlyMoveConvertiblePair()
  {
    return __and_<__or_<is_same<typename decay<_T1>::type,
				typename decay<_U1>::type>,
			is_convertible<_U1&&, _T1>>,
		  __or_<is_same<typename decay<_T2>::type,
				typename decay<_U2>::type>,
		       is_convertible<_U2&&, _T2>>>::value;
  }



 




 
  template<typename _T1, typename _T2>
    struct pair
    {
      typedef _T1 first_type;    
      typedef _T2 second_type;   

      _T1 first;                 
      _T2 second;                

      
      
      
 
      template <typename _U1 = _T1,
                typename _U2 = _T2,
                typename enable_if<__and_<
                                     __is_implicitly_default_constructible<_U1>,
                                     __is_implicitly_default_constructible<_U2>>
                                   ::value, bool>::type = true>
      constexpr pair()
      : first(), second() { }

      template <typename _U1 = _T1,
                typename _U2 = _T2,
                typename enable_if<__and_<
                       is_default_constructible<_U1>,
                       is_default_constructible<_U2>,
                       __not_<
                         __and_<__is_implicitly_default_constructible<_U1>,
                                __is_implicitly_default_constructible<_U2>>>>
                                   ::value, bool>::type = false>
      explicit constexpr pair()
      : first(), second() { }

       
      template<typename _U1 = _T1, typename _U2=_T2, typename
                enable_if<_ConstructiblePair<_T1, _T2, _U1, _U2>()
                         && _ImplicitlyConvertiblePair<_T1, _T2, _U1, _U2>(),
                         bool>::type=true>
      constexpr pair(const _T1& __a, const _T2& __b)
      : first(__a), second(__b) { }

       template<typename _U1 = _T1, typename _U2=_T2, typename
	       enable_if<_ConstructiblePair<_T1, _T2, _U1, _U2>()
                         && !_ImplicitlyConvertiblePair<_T1, _T2, _U1, _U2>(),
                         bool>::type=false>
      explicit constexpr pair(const _T1& __a, const _T2& __b)
      : first(__a), second(__b) { }

       
      template<typename _U1, typename _U2, typename
	       enable_if<_ConstructiblePair<_T1, _T2, _U1, _U2>()
                         && _ImplicitlyConvertiblePair<_T1, _T2, _U1, _U2>(),
                         bool>::type=true>
        constexpr pair(const pair<_U1, _U2>& __p)
        : first(__p.first), second(__p.second) { }

      template<typename _U1, typename _U2, typename
               enable_if<_ConstructiblePair<_T1, _T2, _U1, _U2>()
                         && !_ImplicitlyConvertiblePair<_T1, _T2, _U1, _U2>(),
                         bool>::type=false>
	explicit constexpr pair(const pair<_U1, _U2>& __p)
	: first(__p.first), second(__p.second) { }

      constexpr pair(const pair&) = default;
      constexpr pair(pair&&) = default;

      
      template<typename _U1, typename
               enable_if<_ConstructiblePair<_T2, _T2, _T2, _T2>()
                         && _MoveConstructiblePair<_T1, _T2, _U1, _T2>()
                         && _ImplicitlyConvertiblePair<_T2, _T2, _T2, _T2>()
                         && _ImplicitlyMoveConvertiblePair<_T1, _T2,
							  _U1, _T2>(),
                         bool>::type=true>
       constexpr pair(_U1&& __x, const _T2& __y)
       : first(std::forward<_U1>(__x)), second(__y) { }

      template<typename _U1, typename
               enable_if<_ConstructiblePair<_T2, _T2, _T2, _T2>()
                         && _MoveConstructiblePair<_T1, _T2, _U1, _T2>()
                         && (!_ImplicitlyConvertiblePair<_T2, _T2, _T2, _T2>()
                             || !_ImplicitlyMoveConvertiblePair<_T1, _T2,
                                                                _U1, _T2>()),
                         bool>::type=false>
       explicit constexpr pair(_U1&& __x, const _T2& __y)
       : first(std::forward<_U1>(__x)), second(__y) { }

      template<typename _U2, typename
               enable_if<_ConstructiblePair<_T1, _T1, _T1, _T1>()
                         && _MoveConstructiblePair<_T1, _T2, _T1, _U2>()
                         && _ImplicitlyConvertiblePair<_T1, _T1, _T1, _T1>()
                         && _ImplicitlyMoveConvertiblePair<_T1, _T2,
                                                           _T1, _U2>(),
                         bool>::type=true>
       constexpr pair(const _T1& __x, _U2&& __y)
       : first(__x), second(std::forward<_U2>(__y)) { }

      template<typename _U2, typename
               enable_if<_ConstructiblePair<_T1, _T1, _T1, _T1>()
                         && _MoveConstructiblePair<_T1, _T2, _T1, _U2>()
                         && (!_ImplicitlyConvertiblePair<_T1, _T1, _T1, _T1>()
                             || !_ImplicitlyMoveConvertiblePair<_T1, _T2,
                                                                _T1, _U2>()),
                         bool>::type=false>
       explicit pair(const _T1& __x, _U2&& __y)
       : first(__x), second(std::forward<_U2>(__y)) { }

      template<typename _U1, typename _U2, typename
	       enable_if<_MoveConstructiblePair<_T1, _T2, _U1, _U2>()
                         && _ImplicitlyMoveConvertiblePair<_T1, _T2,
							   _U1, _U2>(),
                         bool>::type=true>
	constexpr pair(_U1&& __x, _U2&& __y)
	: first(std::forward<_U1>(__x)), second(std::forward<_U2>(__y)) { }

      template<typename _U1, typename _U2, typename
	       enable_if<_MoveConstructiblePair<_T1, _T2, _U1, _U2>()
                         && !_ImplicitlyMoveConvertiblePair<_T1, _T2,
							    _U1, _U2>(),
                         bool>::type=false>
	explicit constexpr pair(_U1&& __x, _U2&& __y)
	: first(std::forward<_U1>(__x)), second(std::forward<_U2>(__y)) { }


      template<typename _U1, typename _U2, typename
	       enable_if<_MoveConstructiblePair<_T1, _T2, _U1, _U2>()
                         && _ImplicitlyMoveConvertiblePair<_T1, _T2,
							   _U1, _U2>(),
                         bool>::type=true>
	constexpr pair(pair<_U1, _U2>&& __p)
	: first(std::forward<_U1>(__p.first)),
	  second(std::forward<_U2>(__p.second)) { }

      template<typename _U1, typename _U2, typename
	       enable_if<_MoveConstructiblePair<_T1, _T2, _U1, _U2>()
                         && !_ImplicitlyMoveConvertiblePair<_T1, _T2,
							   _U1, _U2>(),
                         bool>::type=false>
	explicit constexpr pair(pair<_U1, _U2>&& __p)
	: first(std::forward<_U1>(__p.first)),
	  second(std::forward<_U2>(__p.second)) { }

      template<typename... _Args1, typename... _Args2>
        pair(piecewise_construct_t, tuple<_Args1...>, tuple<_Args2...>);

      pair&
      operator=(const pair& __p)
      {
	first = __p.first;
	second = __p.second;
	return *this;
      }

      pair&
      operator=(pair&& __p)
      noexcept(__and_<is_nothrow_move_assignable<_T1>,
	              is_nothrow_move_assignable<_T2>>::value)
      {
	first = std::forward<first_type>(__p.first);
	second = std::forward<second_type>(__p.second);
	return *this;
      }

      template<typename _U1, typename _U2>
	pair&
	operator=(const pair<_U1, _U2>& __p)
	{
	  first = __p.first;
	  second = __p.second;
	  return *this;
	}

      template<typename _U1, typename _U2>
	pair&
	operator=(pair<_U1, _U2>&& __p)
	{
	  first = std::forward<_U1>(__p.first);
	  second = std::forward<_U2>(__p.second);
	  return *this;
	}

      void
      swap(pair& __p)
      noexcept(__is_nothrow_swappable<_T1>::value
               && __is_nothrow_swappable<_T2>::value)
      {
	using std::swap;
	swap(first, __p.first);
	swap(second, __p.second);
      }

    private:
      template<typename... _Args1, std::size_t... _Indexes1,
               typename... _Args2, std::size_t... _Indexes2>
        pair(tuple<_Args1...>&, tuple<_Args2...>&,
             _Index_tuple<_Indexes1...>, _Index_tuple<_Indexes2...>);
    };

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator==(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first == __y.first && __x.second == __y.second; }

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator<(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __x.first < __y.first
	     || (!(__y.first < __x.first) && __x.second < __y.second); }

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator!=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x == __y); }

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator>(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return __y < __x; }

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator<=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__y < __x); }

  
  template<typename _T1, typename _T2>
    inline constexpr bool
    operator>=(const pair<_T1, _T2>& __x, const pair<_T1, _T2>& __y)
    { return !(__x < __y); }

  
  
  
  template<typename _T1, typename _T2>
    inline void
    swap(pair<_T1, _T2>& __x, pair<_T1, _T2>& __y)
    noexcept(noexcept(__x.swap(__y)))
    { __x.swap(__y); }

  
  template<typename _T1, typename _T2>
    constexpr pair<typename __decay_and_strip<_T1>::__type,
                   typename __decay_and_strip<_T2>::__type>
    make_pair(_T1&& __x, _T2&& __y)
    {
      typedef typename __decay_and_strip<_T1>::__type __ds_type1;
      typedef typename __decay_and_strip<_T2>::__type __ds_type2;
      typedef pair<__ds_type1, __ds_type2> 	      __pair_type;
      return __pair_type(std::forward<_T1>(__x), std::forward<_T2>(__y));
    }

} 


namespace std 
{
  
  struct input_iterator_tag { };

  
  struct output_iterator_tag { };

  
  struct forward_iterator_tag : public input_iterator_tag { };

  
  
  struct bidirectional_iterator_tag : public forward_iterator_tag { };

  
  
  struct random_access_iterator_tag : public bidirectional_iterator_tag { };
  
  template<typename _Category, typename _Tp, typename _Distance = ptrdiff_t,
           typename _Pointer = _Tp*, typename _Reference = _Tp&>
    struct iterator
    {
      
      typedef _Category  iterator_category;
      
      typedef _Tp        value_type;
      
      typedef _Distance  difference_type;
      
      typedef _Pointer   pointer;
      
      typedef _Reference reference;
    };

  
  template<typename _Iterator, typename = __void_t<>>
    struct __iterator_traits { };

  template<typename _Iterator>
    struct __iterator_traits<_Iterator,
			     __void_t<typename _Iterator::iterator_category,
				      typename _Iterator::value_type,
				      typename _Iterator::difference_type,
				      typename _Iterator::pointer,
				      typename _Iterator::reference>>
    {
      typedef typename _Iterator::iterator_category iterator_category;
      typedef typename _Iterator::value_type        value_type;
      typedef typename _Iterator::difference_type   difference_type;
      typedef typename _Iterator::pointer           pointer;
      typedef typename _Iterator::reference         reference;
    };

  template<typename _Iterator>
    struct iterator_traits
    : public __iterator_traits<_Iterator> { };

  
  template<typename _Tp>
    struct iterator_traits<_Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef _Tp*                        pointer;
      typedef _Tp&                        reference;
    };

  
  template<typename _Tp>
    struct iterator_traits<const _Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef const _Tp*                  pointer;
      typedef const _Tp&                  reference;
    };

  


 
  template<typename _Iter>
    inline typename iterator_traits<_Iter>::iterator_category
    __iterator_category(const _Iter&)
    { return typename iterator_traits<_Iter>::iterator_category(); }

  


  template<typename _InIter>
    using _RequireInputIter = typename
      enable_if<is_convertible<typename
		iterator_traits<_InIter>::iterator_category,
			       input_iterator_tag>::value>::type;


} 


namespace std 
{
 
  template <typename> struct _List_iterator;
  template <typename> struct _List_const_iterator;

  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    __distance(_InputIterator __first, _InputIterator __last,
               input_iterator_tag)
    {
      typename iterator_traits<_InputIterator>::difference_type __n = 0;
      while (__first != __last)
	{
	  ++__first;
	  ++__n;
	}
      return __n;
    }

  template<typename _RandomAccessIterator>
    inline typename iterator_traits<_RandomAccessIterator>::difference_type
    __distance(_RandomAccessIterator __first, _RandomAccessIterator __last,
               random_access_iterator_tag)
    {
      
      
      return __last - __first;
    }

  
  template<typename _Tp>
    ptrdiff_t
    __distance(std::_List_iterator<_Tp>,
	       std::_List_iterator<_Tp>,
	       input_iterator_tag);

  template<typename _Tp>
    ptrdiff_t
    __distance(std::_List_const_iterator<_Tp>,
	       std::_List_const_iterator<_Tp>,
	       input_iterator_tag);

 
  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    distance(_InputIterator __first, _InputIterator __last)
    {
      
      return std::__distance(__first, __last,
			     std::__iterator_category(__first));
    }

  template<typename _InputIterator, typename _Distance>
    inline void
    __advance(_InputIterator& __i, _Distance __n, input_iterator_tag)
    {
      
      
      ;
      while (__n--)
	++__i;
    }

  template<typename _BidirectionalIterator, typename _Distance>
    inline void
    __advance(_BidirectionalIterator& __i, _Distance __n,
	      bidirectional_iterator_tag)
    {
      
      
      if (__n > 0)
        while (__n--)
	  ++__i;
      else
        while (__n++)
	  --__i;
    }

  template<typename _RandomAccessIterator, typename _Distance>
    inline void
    __advance(_RandomAccessIterator& __i, _Distance __n,
              random_access_iterator_tag)
    {
      
      
      __i += __n;
    }

  template<typename _InputIterator, typename _Distance>
    inline void
    advance(_InputIterator& __i, _Distance __n)
    {
      
      typename iterator_traits<_InputIterator>::difference_type __d = __n;
      std::__advance(__i, __d, std::__iterator_category(__i));
    }


  template<typename _ForwardIterator>
    inline _ForwardIterator
    next(_ForwardIterator __x, typename
	 iterator_traits<_ForwardIterator>::difference_type __n = 1)
    {
      
      
      std::advance(__x, __n);
      return __x;
    }

  template<typename _BidirectionalIterator>
    inline _BidirectionalIterator
    prev(_BidirectionalIterator __x, typename
	 iterator_traits<_BidirectionalIterator>::difference_type __n = 1) 
    {
      
      
      std::advance(__x, -__n);
      return __x;
    }
} 


namespace std 
{


  class __undefined;

  
  template<typename _Tp>
    struct __get_first_arg
    { using type = __undefined; };

  template<template<typename, typename...> class _Template, typename _Tp,
           typename... _Types>
    struct __get_first_arg<_Template<_Tp, _Types...>>
    { using type = _Tp; };

  template<typename _Tp>
    using __get_first_arg_t = typename __get_first_arg<_Tp>::type;

  
  template<typename _Tp, typename _Up>
    struct __replace_first_arg
    { using type = __undefined; };

  template<template<typename, typename...> class _Template, typename _Up,
           typename _Tp, typename... _Types>
    struct __replace_first_arg<_Template<_Tp, _Types...>, _Up>
    { using type = _Template<_Up, _Types...>; };

  template<typename _Tp, typename _Up>
    using __replace_first_arg_t = typename __replace_first_arg<_Tp, _Up>::type;

  template<typename _Tp>
    using __make_not_void
      = typename conditional<is_void<_Tp>::value, __undefined, _Tp>::type;

  


 
  template<typename _Ptr>
    struct pointer_traits
    {
    private:
      template<typename _Tp>
	using __element_type = typename _Tp::element_type;

      template<typename _Tp>
	using __difference_type = typename _Tp::difference_type;

      template<typename _Tp, typename _Up>
	using __rebind = typename _Tp::template rebind<_Up>;

    public:
      
      using pointer = _Ptr;

      
      using element_type
	= __detected_or_t_<__get_first_arg_t, __element_type, _Ptr>;

      
      using difference_type
	= __detected_or_t<ptrdiff_t, __difference_type, _Ptr>;

      
      template<typename _Up>
        using rebind
	  = __detected_or_t_<__replace_first_arg_t, __rebind, _Ptr, _Up>;

      static _Ptr
      pointer_to(__make_not_void<element_type>& __e)
      { return _Ptr::pointer_to(__e); }

      static_assert(!is_same<element_type, __undefined>::value,
	  "pointer type defines element_type or is like SomePointer<T, Args>");
      static_assert(!is_same<rebind<element_type>, __undefined>::value,
	  "pointer type defines rebind<U> or is like SomePointer<T, Args>");
    };

  


 
  template<typename _Tp>
    struct pointer_traits<_Tp*>
    {
      
      typedef _Tp* pointer;
      
      typedef _Tp  element_type;
      
      typedef ptrdiff_t difference_type;

      template<typename _Up>
        using rebind = _Up*;

      



 
      static pointer
      pointer_to(__make_not_void<element_type>& __r) noexcept
      { return std::addressof(__r); }
    };

  
  template<typename _Ptr, typename _Tp>
    using __ptr_rebind = typename pointer_traits<_Ptr>::template rebind<_Tp>;


} 



namespace std 
{
 
  template<typename _Iterator>
    class reverse_iterator
    : public iterator<typename iterator_traits<_Iterator>::iterator_category,
		      typename iterator_traits<_Iterator>::value_type,
		      typename iterator_traits<_Iterator>::difference_type,
		      typename iterator_traits<_Iterator>::pointer,
                      typename iterator_traits<_Iterator>::reference>
    {
    protected:
      _Iterator current;

      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::difference_type	difference_type;
      typedef typename __traits_type::pointer		pointer;
      typedef typename __traits_type::reference		reference;

      reverse_iterator() : current() { }
 
      explicit
      reverse_iterator(iterator_type __x) : current(__x) { }

      

 
      reverse_iterator(const reverse_iterator& __x)
      : current(__x.current) { }

      


 
      template<typename _Iter>
        reverse_iterator(const reverse_iterator<_Iter>& __x)
	: current(__x.base()) { }

      

 
      iterator_type
      base() const
      { return current; }

 
      reference
      operator*() const
      {
	_Iterator __tmp = current;
	return *--__tmp;
      }

      pointer
      operator->() const
      { return &(operator*()); }

 
      reverse_iterator&
      operator++()
      {
	--current;
	return *this;
      }

      



 
      reverse_iterator
      operator++(int)
      {
	reverse_iterator __tmp = *this;
	--current;
	return __tmp;
      }

      



 
      reverse_iterator&
      operator--()
      {
	++current;
	return *this;
      }

      



 
      reverse_iterator
      operator--(int)
      {
	reverse_iterator __tmp = *this;
	++current;
	return __tmp;
      }

      



 
      reverse_iterator
      operator+(difference_type __n) const
      { return reverse_iterator(current - __n); }

      




 
      reverse_iterator&
      operator+=(difference_type __n)
      {
	current -= __n;
	return *this;
      }

      



 
      reverse_iterator
      operator-(difference_type __n) const
      { return reverse_iterator(current + __n); }

      




 
      reverse_iterator&
      operator-=(difference_type __n)
      {
	current += __n;
	return *this;
      }

      



 
      reference
      operator[](difference_type __n) const
      { return *(*this + __n); }
    };

  
  







 
  template<typename _Iterator>
    inline bool
    operator==(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return __x.base() == __y.base(); }

  template<typename _Iterator>
    inline bool
    operator<(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y.base() < __x.base(); }

  template<typename _Iterator>
    inline bool
    operator!=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__x == __y); }

  template<typename _Iterator>
    inline bool
    operator>(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y < __x; }

  template<typename _Iterator>
    inline bool
    operator<=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__y < __x); }

  template<typename _Iterator>
    inline bool
    operator>=(const reverse_iterator<_Iterator>& __x,
	       const reverse_iterator<_Iterator>& __y)
    { return !(__x < __y); }

  template<typename _Iterator>
    inline typename reverse_iterator<_Iterator>::difference_type
    operator-(const reverse_iterator<_Iterator>& __x,
	      const reverse_iterator<_Iterator>& __y)
    { return __y.base() - __x.base(); }

  template<typename _Iterator>
    inline reverse_iterator<_Iterator>
    operator+(typename reverse_iterator<_Iterator>::difference_type __n,
	      const reverse_iterator<_Iterator>& __x)
    { return reverse_iterator<_Iterator>(__x.base() - __n); }

  
  
  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator==(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return __x.base() == __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    { return __y.base() < __x.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator!=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__x == __y); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    { return __y < __x; }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__y < __x); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>=(const reverse_iterator<_IteratorL>& __x,
	       const reverse_iterator<_IteratorR>& __y)
    { return !(__x < __y); }

  template<typename _IteratorL, typename _IteratorR>
    
    inline auto
    operator-(const reverse_iterator<_IteratorL>& __x,
	      const reverse_iterator<_IteratorR>& __y)
    -> decltype(__y.base() - __x.base())
    { return __y.base() - __x.base(); }
  

  
  template<typename _Iterator>
    inline reverse_iterator<_Iterator>
    __make_reverse_iterator(_Iterator __i)
    { return reverse_iterator<_Iterator>(__i); }


  
  
  
  template<typename _Iterator>
    inline reverse_iterator<_Iterator>
    make_reverse_iterator(_Iterator __i)
    { return reverse_iterator<_Iterator>(__i); }

  template<typename _Iterator>
    auto
    __niter_base(reverse_iterator<_Iterator> __it)
    -> decltype(__make_reverse_iterator(__niter_base(__it.base())))
    { return __make_reverse_iterator(__niter_base(__it.base())); }

  template<typename _Iterator>
    struct __is_move_iterator<reverse_iterator<_Iterator> >
      : __is_move_iterator<_Iterator>
    { };

  template<typename _Iterator>
    auto
    __miter_base(reverse_iterator<_Iterator> __it)
    -> decltype(__make_reverse_iterator(__miter_base(__it.base())))
    { return __make_reverse_iterator(__miter_base(__it.base())); }

  
  








 
  template<typename _Container>
    class back_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;

    public:
      
      typedef _Container          container_type;

      
      explicit
      back_insert_iterator(_Container& __x)
      : container(std::__addressof(__x)) { }

      









 
      back_insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	container->push_back(__value);
	return *this;
      }

      back_insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	container->push_back(std::move(__value));
	return *this;
      }

      
      back_insert_iterator&
      operator*()
      { return *this; }

      
      back_insert_iterator&
      operator++()
      { return *this; }

      
      back_insert_iterator
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container>
    inline back_insert_iterator<_Container>
    back_inserter(_Container& __x)
    { return back_insert_iterator<_Container>(__x); }

  








 
  template<typename _Container>
    class front_insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;

    public:
      
      typedef _Container          container_type;

      
      explicit front_insert_iterator(_Container& __x)
      : container(std::__addressof(__x)) { }

      









 
      front_insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	container->push_front(__value);
	return *this;
      }

      front_insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	container->push_front(std::move(__value));
	return *this;
      }

      
      front_insert_iterator&
      operator*()
      { return *this; }

      
      front_insert_iterator&
      operator++()
      { return *this; }

      
      front_insert_iterator
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container>
    inline front_insert_iterator<_Container>
    front_inserter(_Container& __x)
    { return front_insert_iterator<_Container>(__x); }

  












 
  template<typename _Container>
    class insert_iterator
    : public iterator<output_iterator_tag, void, void, void, void>
    {
    protected:
      _Container* container;
      typename _Container::iterator iter;

    public:
      
      typedef _Container          container_type;

      


 
      insert_iterator(_Container& __x, typename _Container::iterator __i)
      : container(std::__addressof(__x)), iter(__i) {}

      





















 
      insert_iterator&
      operator=(const typename _Container::value_type& __value)
      {
	iter = container->insert(iter, __value);
	++iter;
	return *this;
      }

      insert_iterator&
      operator=(typename _Container::value_type&& __value)
      {
	iter = container->insert(iter, std::move(__value));
	++iter;
	return *this;
      }

      
      insert_iterator&
      operator*()
      { return *this; }

      
      insert_iterator&
      operator++()
      { return *this; }

      
      insert_iterator&
      operator++(int)
      { return *this; }
    };

  









 
  template<typename _Container, typename _Iterator>
    inline insert_iterator<_Container>
    inserter(_Container& __x, _Iterator __i)
    {
      return insert_iterator<_Container>(__x,
					 typename _Container::iterator(__i));
    }

  


} 

namespace __gnu_cxx 
{


  
  
  
  
  
  
  
  using std::iterator_traits;
  using std::iterator;
  template<typename _Iterator, typename _Container>
    class __normal_iterator
    {
    protected:
      _Iterator _M_current;

      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::iterator_category iterator_category;
      typedef typename __traits_type::value_type  	value_type;
      typedef typename __traits_type::difference_type 	difference_type;
      typedef typename __traits_type::reference 	reference;
      typedef typename __traits_type::pointer   	pointer;

      constexpr __normal_iterator() noexcept
      : _M_current(_Iterator()) { }

      explicit
      __normal_iterator(const _Iterator& __i) noexcept
      : _M_current(__i) { }

      
      template<typename _Iter>
        __normal_iterator(const __normal_iterator<_Iter,
			  typename __enable_if<
      	       (std::__are_same<_Iter, typename _Container::pointer>::__value),
		      _Container>::__type>& __i) noexcept
        : _M_current(__i.base()) { }

      
      reference
      operator*() const noexcept
      { return *_M_current; }

      pointer
      operator->() const noexcept
      { return _M_current; }

      __normal_iterator&
      operator++() noexcept
      {
	++_M_current;
	return *this;
      }

      __normal_iterator
      operator++(int) noexcept
      { return __normal_iterator(_M_current++); }

      
      __normal_iterator&
      operator--() noexcept
      {
	--_M_current;
	return *this;
      }

      __normal_iterator
      operator--(int) noexcept
      { return __normal_iterator(_M_current--); }

      
      reference
      operator[](difference_type __n) const noexcept
      { return _M_current[__n]; }

      __normal_iterator&
      operator+=(difference_type __n) noexcept
      { _M_current += __n; return *this; }

      __normal_iterator
      operator+(difference_type __n) const noexcept
      { return __normal_iterator(_M_current + __n); }

      __normal_iterator&
      operator-=(difference_type __n) noexcept
      { _M_current -= __n; return *this; }

      __normal_iterator
      operator-(difference_type __n) const noexcept
      { return __normal_iterator(_M_current - __n); }

      const _Iterator&
      base() const noexcept
      { return _M_current; }
    };

  
  
  
  
  
  
  

  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator==(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() == __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator==(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() == __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() != __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator!=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() != __rhs.base(); }

  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() < __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() < __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() > __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() > __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() <= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator<=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() <= __rhs.base(); }

  template<typename _IteratorL, typename _IteratorR, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_IteratorL, _Container>& __lhs,
	       const __normal_iterator<_IteratorR, _Container>& __rhs)
    noexcept
    { return __lhs.base() >= __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline bool
    operator>=(const __normal_iterator<_Iterator, _Container>& __lhs,
	       const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() >= __rhs.base(); }

  
  
  
  
  template<typename _IteratorL, typename _IteratorR, typename _Container>
    
    inline auto
    operator-(const __normal_iterator<_IteratorL, _Container>& __lhs,
	      const __normal_iterator<_IteratorR, _Container>& __rhs) noexcept
    -> decltype(__lhs.base() - __rhs.base())
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline typename __normal_iterator<_Iterator, _Container>::difference_type
    operator-(const __normal_iterator<_Iterator, _Container>& __lhs,
	      const __normal_iterator<_Iterator, _Container>& __rhs)
    noexcept
    { return __lhs.base() - __rhs.base(); }

  template<typename _Iterator, typename _Container>
    inline __normal_iterator<_Iterator, _Container>
    operator+(typename __normal_iterator<_Iterator, _Container>::difference_type
	      __n, const __normal_iterator<_Iterator, _Container>& __i)
    noexcept
    { return __normal_iterator<_Iterator, _Container>(__i.base() + __n); }


} 

namespace std 
{


  template<typename _Iterator, typename _Container>
    _Iterator
    __niter_base(__gnu_cxx::__normal_iterator<_Iterator, _Container> __it)
    { return __it.base(); }


} 


namespace std 
{
 
  template<typename _Iterator>
    class move_iterator
    {
    protected:
      _Iterator _M_current;

      typedef iterator_traits<_Iterator>		__traits_type;
      typedef typename __traits_type::reference		__base_ref;

    public:
      typedef _Iterator					iterator_type;
      typedef typename __traits_type::iterator_category iterator_category;
      typedef typename __traits_type::value_type  	value_type;
      typedef typename __traits_type::difference_type	difference_type;
      
      typedef _Iterator					pointer;
      
      
      typedef typename conditional<is_reference<__base_ref>::value,
			 typename remove_reference<__base_ref>::type&&,
			 __base_ref>::type		reference;

      move_iterator()
      : _M_current() { }

      explicit
      move_iterator(iterator_type __i)
      : _M_current(__i) { }

      template<typename _Iter>
	move_iterator(const move_iterator<_Iter>& __i)
	: _M_current(__i.base()) { }

      iterator_type
      base() const
      { return _M_current; }

      reference
      operator*() const
      { return static_cast<reference>(*_M_current); }

      pointer
      operator->() const
      { return _M_current; }

      move_iterator&
      operator++()
      {
	++_M_current;
	return *this;
      }

      move_iterator
      operator++(int)
      {
	move_iterator __tmp = *this;
	++_M_current;
	return __tmp;
      }

      move_iterator&
      operator--()
      {
	--_M_current;
	return *this;
      }

      move_iterator
      operator--(int)
      {
	move_iterator __tmp = *this;
	--_M_current;
	return __tmp;
      }

      move_iterator
      operator+(difference_type __n) const
      { return move_iterator(_M_current + __n); }

      move_iterator&
      operator+=(difference_type __n)
      {
	_M_current += __n;
	return *this;
      }

      move_iterator
      operator-(difference_type __n) const
      { return move_iterator(_M_current - __n); }
    
      move_iterator&
      operator-=(difference_type __n)
      { 
	_M_current -= __n;
	return *this;
      }

      reference
      operator[](difference_type __n) const
      { return std::move(_M_current[__n]); }
    };

  
  
  
  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator==(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return __x.base() == __y.base(); }

  template<typename _Iterator>
    inline bool
    operator==(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return __x.base() == __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator!=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__x == __y); }

  template<typename _Iterator>
    inline bool
    operator!=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__x == __y); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    { return __x.base() < __y.base(); }

  template<typename _Iterator>
    inline bool
    operator<(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    { return __x.base() < __y.base(); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator<=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__y < __x); }

  template<typename _Iterator>
    inline bool
    operator<=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__y < __x); }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    { return __y < __x; }

  template<typename _Iterator>
    inline bool
    operator>(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    { return __y < __x; }

  template<typename _IteratorL, typename _IteratorR>
    inline bool
    operator>=(const move_iterator<_IteratorL>& __x,
	       const move_iterator<_IteratorR>& __y)
    { return !(__x < __y); }

  template<typename _Iterator>
    inline bool
    operator>=(const move_iterator<_Iterator>& __x,
	       const move_iterator<_Iterator>& __y)
    { return !(__x < __y); }

  
  template<typename _IteratorL, typename _IteratorR>
    inline auto
    operator-(const move_iterator<_IteratorL>& __x,
	      const move_iterator<_IteratorR>& __y)
    -> decltype(__x.base() - __y.base())
    { return __x.base() - __y.base(); }

  template<typename _Iterator>
    inline auto
    operator-(const move_iterator<_Iterator>& __x,
	      const move_iterator<_Iterator>& __y)
    -> decltype(__x.base() - __y.base())
    { return __x.base() - __y.base(); }

  template<typename _Iterator>
    inline move_iterator<_Iterator>
    operator+(typename move_iterator<_Iterator>::difference_type __n,
	      const move_iterator<_Iterator>& __x)
    { return __x + __n; }

  template<typename _Iterator>
    inline move_iterator<_Iterator>
    make_move_iterator(_Iterator __i)
    { return move_iterator<_Iterator>(__i); }

  template<typename _Iterator, typename _ReturnType
    = typename conditional<__move_if_noexcept_cond
      <typename iterator_traits<_Iterator>::value_type>::value,
                _Iterator, move_iterator<_Iterator>>::type>
    inline _ReturnType
    __make_move_if_noexcept_iterator(_Iterator __i)
    { return _ReturnType(__i); }

  
  
  template<typename _Tp, typename _ReturnType
    = typename conditional<__move_if_noexcept_cond<_Tp>::value,
			   const _Tp*, move_iterator<_Tp*>>::type>
    inline _ReturnType
    __make_move_if_noexcept_iterator(_Tp* __i)
    { return _ReturnType(__i); }

  

  template<typename _Iterator>
    auto
    __niter_base(move_iterator<_Iterator> __it)
    -> decltype(make_move_iterator(__niter_base(__it.base())))
    { return make_move_iterator(__niter_base(__it.base())); }

  template<typename _Iterator>
    struct __is_move_iterator<move_iterator<_Iterator> >
    {
      enum { __value = 1 };
      typedef __true_type __type;
    };

  template<typename _Iterator>
    auto
    __miter_base(move_iterator<_Iterator> __it)
    -> decltype(__miter_base(__it.base()))
    { return __miter_base(__it.base()); }


} 

 
namespace std
{ 
  namespace __debug { } 
}



 
namespace __gnu_debug
{
  using namespace std::__debug;
}


namespace __gnu_cxx
{
namespace __ops
{
  struct _Iter_less_iter
  {
    template<typename _Iterator1, typename _Iterator2>
      constexpr
      bool
      operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return *__it1 < *__it2; }
  };
  constexpr
  inline _Iter_less_iter
  __iter_less_iter()
  { return _Iter_less_iter(); }

  struct _Iter_less_val
  {
    template<typename _Iterator, typename _Value>
      bool
      operator()(_Iterator __it, _Value& __val) const
      { return *__it < __val; }
    };

  inline _Iter_less_val
  __iter_less_val()
  { return _Iter_less_val(); }

  inline _Iter_less_val
  __iter_comp_val(_Iter_less_iter)
  { return _Iter_less_val(); }

  struct _Val_less_iter
  {
    template<typename _Value, typename _Iterator>
      bool
      operator()(_Value& __val, _Iterator __it) const
      { return __val < *__it; }
    };

  inline _Val_less_iter
  __val_less_iter()
  { return _Val_less_iter(); }

  inline _Val_less_iter
  __val_comp_iter(_Iter_less_iter)
  { return _Val_less_iter(); }

  struct _Iter_equal_to_iter
  {
    template<typename _Iterator1, typename _Iterator2>
      bool
      operator()(_Iterator1 __it1, _Iterator2 __it2) const
      { return *__it1 == *__it2; }
    };

  inline _Iter_equal_to_iter
  __iter_equal_to_iter()
  { return _Iter_equal_to_iter(); }

  struct _Iter_equal_to_val
  {
    template<typename _Iterator, typename _Value>
      bool
      operator()(_Iterator __it, _Value& __val) const
      { return *__it == __val; }
    };

  inline _Iter_equal_to_val
  __iter_equal_to_val()
  { return _Iter_equal_to_val(); }

  inline _Iter_equal_to_val
  __iter_comp_val(_Iter_equal_to_iter)
  { return _Iter_equal_to_val(); }

  template<typename _Compare>
    struct _Iter_comp_iter
    {
      _Compare _M_comp;
      constexpr
      _Iter_comp_iter(_Compare __comp)
	: _M_comp(__comp)
      { }

      template<typename _Iterator1, typename _Iterator2>
        constexpr
        bool
        operator()(_Iterator1 __it1, _Iterator2 __it2)
        { return bool(_M_comp(*__it1, *__it2)); }
    };

  template<typename _Compare>
    constexpr
    inline _Iter_comp_iter<_Compare>
    __iter_comp_iter(_Compare __comp)
    { return _Iter_comp_iter<_Compare>(__comp); }

  template<typename _Compare>
    struct _Iter_comp_val
    {
      _Compare _M_comp;

      _Iter_comp_val(_Compare __comp)
	: _M_comp(__comp)
      { }

      template<typename _Iterator, typename _Value>
	bool
	operator()(_Iterator __it, _Value& __val)
	{ return bool(_M_comp(*__it, __val)); }
    };

  template<typename _Compare>
   inline _Iter_comp_val<_Compare>
    __iter_comp_val(_Compare __comp)
    { return _Iter_comp_val<_Compare>(__comp); }

  template<typename _Compare>
    inline _Iter_comp_val<_Compare>
    __iter_comp_val(_Iter_comp_iter<_Compare> __comp)
    { return _Iter_comp_val<_Compare>(__comp._M_comp); }

  template<typename _Compare>
    struct _Val_comp_iter
    {
      _Compare _M_comp;

      _Val_comp_iter(_Compare __comp)
	: _M_comp(__comp)
      { }

      template<typename _Value, typename _Iterator>
	bool
	operator()(_Value& __val, _Iterator __it)
	{ return bool(_M_comp(__val, *__it)); }
    };

  template<typename _Compare>
    inline _Val_comp_iter<_Compare>
    __val_comp_iter(_Compare __comp)
    { return _Val_comp_iter<_Compare>(__comp); }

  template<typename _Compare>
    inline _Val_comp_iter<_Compare>
    __val_comp_iter(_Iter_comp_iter<_Compare> __comp)
    { return _Val_comp_iter<_Compare>(__comp._M_comp); }

  template<typename _Value>
    struct _Iter_equals_val
    {
      _Value& _M_value;

      _Iter_equals_val(_Value& __value)
	: _M_value(__value)
      { }

      template<typename _Iterator>
	bool
	operator()(_Iterator __it)
	{ return *__it == _M_value; }
    };

  template<typename _Value>
    inline _Iter_equals_val<_Value>
    __iter_equals_val(_Value& __val)
    { return _Iter_equals_val<_Value>(__val); }

  template<typename _Iterator1>
    struct _Iter_equals_iter
    {
      typename std::iterator_traits<_Iterator1>::reference _M_ref;

      _Iter_equals_iter(_Iterator1 __it1)
	: _M_ref(*__it1)
      { }

      template<typename _Iterator2>
	bool
	operator()(_Iterator2 __it2)
	{ return *__it2 == _M_ref; }
    };

  template<typename _Iterator>
    inline _Iter_equals_iter<_Iterator>
    __iter_comp_iter(_Iter_equal_to_iter, _Iterator __it)
    { return _Iter_equals_iter<_Iterator>(__it); }

  template<typename _Predicate>
    struct _Iter_pred
    {
      _Predicate _M_pred;

      _Iter_pred(_Predicate __pred)
	: _M_pred(__pred)
      { }

      template<typename _Iterator>
	bool
	operator()(_Iterator __it)
	{ return bool(_M_pred(*__it)); }
    };

  template<typename _Predicate>
    inline _Iter_pred<_Predicate>
    __pred_iter(_Predicate __pred)
    { return _Iter_pred<_Predicate>(__pred); }

  template<typename _Compare, typename _Value>
    struct _Iter_comp_to_val
    {
      _Compare _M_comp;
      _Value& _M_value;

      _Iter_comp_to_val(_Compare __comp, _Value& __value)
	: _M_comp(__comp), _M_value(__value)
      { }

      template<typename _Iterator>
	bool
	operator()(_Iterator __it)
	{ return bool(_M_comp(*__it, _M_value)); }
    };

  template<typename _Compare, typename _Value>
    _Iter_comp_to_val<_Compare, _Value>
    __iter_comp_val(_Compare __comp, _Value &__val)
    { return _Iter_comp_to_val<_Compare, _Value>(__comp, __val); }

  template<typename _Compare, typename _Iterator1>
    struct _Iter_comp_to_iter
    {
      _Compare _M_comp;
      typename std::iterator_traits<_Iterator1>::reference _M_ref;

      _Iter_comp_to_iter(_Compare __comp, _Iterator1 __it1)
	: _M_comp(__comp), _M_ref(*__it1)
      { }

      template<typename _Iterator2>
	bool
	operator()(_Iterator2 __it2)
	{ return bool(_M_comp(*__it2, _M_ref)); }
    };

  template<typename _Compare, typename _Iterator>
    inline _Iter_comp_to_iter<_Compare, _Iterator>
    __iter_comp_iter(_Iter_comp_iter<_Compare> __comp, _Iterator __it)
    { return _Iter_comp_to_iter<_Compare, _Iterator>(__comp._M_comp, __it); }

  template<typename _Predicate>
    struct _Iter_negate
    {
      _Predicate _M_pred;

      _Iter_negate(_Predicate __pred)
	: _M_pred(__pred)
      { }

      template<typename _Iterator>
	bool
	operator()(_Iterator __it)
	{ return !bool(_M_pred(*__it)); }
    };

  template<typename _Predicate>
    inline _Iter_negate<_Predicate>
    __negate(_Iter_pred<_Predicate> __pred)
    { return _Iter_negate<_Predicate>(__pred._M_pred); }

} 
} 


namespace std 
{



  








 
  template<typename _ForwardIterator1, typename _ForwardIterator2>
    inline void
    iter_swap(_ForwardIterator1 __a, _ForwardIterator2 __b)
    {
      
      
      

      swap(*__a, *__b);
    }

  










 
  template<typename _ForwardIterator1, typename _ForwardIterator2>
    _ForwardIterator2
    swap_ranges(_ForwardIterator1 __first1, _ForwardIterator1 __last1,
		_ForwardIterator2 __first2)
    {
      
      
      
      ;

      for (; __first1 != __last1; ++__first1, (void)++__first2)
	std::iter_swap(__first1, __first2);
      return __first2;
    }

  









 
  template<typename _Tp>
    constexpr
    inline const _Tp&
    min(const _Tp& __a, const _Tp& __b)
    {
      
      
      
      if (__b < __a)
	return __b;
      return __a;
    }

  









 
  template<typename _Tp>
    constexpr
    inline const _Tp&
    max(const _Tp& __a, const _Tp& __b)
    {
      
      
      
      if (__a < __b)
	return __b;
      return __a;
    }

  









 
  template<typename _Tp, typename _Compare>
    constexpr
    inline const _Tp&
    min(const _Tp& __a, const _Tp& __b, _Compare __comp)
    {
      
      if (__comp(__b, __a))
	return __b;
      return __a;
    }

  









 
  template<typename _Tp, typename _Compare>
    constexpr
    inline const _Tp&
    max(const _Tp& __a, const _Tp& __b, _Compare __comp)
    {
      
      if (__comp(__a, __b))
	return __b;
      return __a;
    }

  
  
  template<typename _Iterator>
    inline _Iterator
    __niter_base(_Iterator __it)
    { return __it; }

  
  
  
  
  

  template<bool, bool, typename>
    struct __copy_move
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        {
	  for (; __first != __last; ++__result, (void)++__first)
	    *__result = *__first;
	  return __result;
	}
    };

  template<typename _Category>
    struct __copy_move<true, false, _Category>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        {
	  for (; __first != __last; ++__result, (void)++__first)
	    *__result = std::move(*__first);
	  return __result;
	}
    };

  template<>
    struct __copy_move<false, false, random_access_iterator_tag>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        { 
	  typedef typename iterator_traits<_II>::difference_type _Distance;
	  for(_Distance __n = __last - __first; __n > 0; --__n)
	    {
	      *__result = *__first;
	      ++__first;
	      ++__result;
	    }
	  return __result;
	}
    };

  template<>
    struct __copy_move<true, false, random_access_iterator_tag>
    {
      template<typename _II, typename _OI>
        static _OI
        __copy_m(_II __first, _II __last, _OI __result)
        { 
	  typedef typename iterator_traits<_II>::difference_type _Distance;
	  for(_Distance __n = __last - __first; __n > 0; --__n)
	    {
	      *__result = std::move(*__first);
	      ++__first;
	      ++__result;
	    }
	  return __result;
	}
    };

  template<bool _IsMove>
    struct __copy_move<_IsMove, true, random_access_iterator_tag>
    {
      template<typename _Tp>
        static _Tp*
        __copy_m(const _Tp* __first, const _Tp* __last, _Tp* __result)
        {
	  using __assignable = conditional<_IsMove,
					   is_move_assignable<_Tp>,
					   is_copy_assignable<_Tp>>;
	  
	  static_assert( __assignable::type::value, "type is not assignable" );
	  const ptrdiff_t _Num = __last - __first;
	  if (_Num)
	    __builtin_memmove(__result, __first, sizeof(_Tp) * _Num);
	  return __result + _Num;
	}
    };

  template<bool _IsMove, typename _II, typename _OI>
    inline _OI
    __copy_move_a(_II __first, _II __last, _OI __result)
    {
      typedef typename iterator_traits<_II>::value_type _ValueTypeI;
      typedef typename iterator_traits<_OI>::value_type _ValueTypeO;
      typedef typename iterator_traits<_II>::iterator_category _Category;
      const bool __simple = (__is_trivial(_ValueTypeI)
	                     && __is_pointer<_II>::__value
	                     && __is_pointer<_OI>::__value
			     && __are_same<_ValueTypeI, _ValueTypeO>::__value);

      return std::__copy_move<_IsMove, __simple,
	                      _Category>::__copy_m(__first, __last, __result);
    }

  
  
  template<typename _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits>
    class istreambuf_iterator;

  template<typename _CharT, typename _Traits>
    class ostreambuf_iterator;

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value, 
	     ostreambuf_iterator<_CharT, char_traits<_CharT> > >::__type
    __copy_move_a2(_CharT*, _CharT*,
		   ostreambuf_iterator<_CharT, char_traits<_CharT> >);

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value, 
	     ostreambuf_iterator<_CharT, char_traits<_CharT> > >::__type
    __copy_move_a2(const _CharT*, const _CharT*,
		   ostreambuf_iterator<_CharT, char_traits<_CharT> >);

  template<bool _IsMove, typename _CharT>
    typename __gnu_cxx::__enable_if<__is_char<_CharT>::__value,
				    _CharT*>::__type
    __copy_move_a2(istreambuf_iterator<_CharT, char_traits<_CharT> >,
		   istreambuf_iterator<_CharT, char_traits<_CharT> >, _CharT*);

  template<bool _IsMove, typename _II, typename _OI>
    inline _OI
    __copy_move_a2(_II __first, _II __last, _OI __result)
    {
      return _OI(std::__copy_move_a<_IsMove>(std::__niter_base(__first),
					     std::__niter_base(__last),
					     std::__niter_base(__result)));
    }

  















 
  template<typename _II, typename _OI>
    inline _OI
    copy(_II __first, _II __last, _OI __result)
    {
      
      
      
      ;

      return (std::__copy_move_a2<__is_move_iterator<_II>::__value>
	      (std::__miter_base(__first), std::__miter_base(__last),
	       __result));
    }

  















 
  template<typename _II, typename _OI>
    inline _OI
    move(_II __first, _II __last, _OI __result)
    {
      
      
      
      ;

      return std::__copy_move_a2<true>(std::__miter_base(__first),
				       std::__miter_base(__last), __result);
    }


  template<bool, bool, typename>
    struct __copy_move_backward
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  while (__first != __last)
	    *--__result = *--__last;
	  return __result;
	}
    };

  template<typename _Category>
    struct __copy_move_backward<true, false, _Category>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  while (__first != __last)
	    *--__result = std::move(*--__last);
	  return __result;
	}
    };

  template<>
    struct __copy_move_backward<false, false, random_access_iterator_tag>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  typename iterator_traits<_BI1>::difference_type __n;
	  for (__n = __last - __first; __n > 0; --__n)
	    *--__result = *--__last;
	  return __result;
	}
    };

  template<>
    struct __copy_move_backward<true, false, random_access_iterator_tag>
    {
      template<typename _BI1, typename _BI2>
        static _BI2
        __copy_move_b(_BI1 __first, _BI1 __last, _BI2 __result)
        {
	  typename iterator_traits<_BI1>::difference_type __n;
	  for (__n = __last - __first; __n > 0; --__n)
	    *--__result = std::move(*--__last);
	  return __result;
	}
    };

  template<bool _IsMove>
    struct __copy_move_backward<_IsMove, true, random_access_iterator_tag>
    {
      template<typename _Tp>
        static _Tp*
        __copy_move_b(const _Tp* __first, const _Tp* __last, _Tp* __result)
        {
	  using __assignable = conditional<_IsMove,
					   is_move_assignable<_Tp>,
					   is_copy_assignable<_Tp>>;
	  
	  static_assert( __assignable::type::value, "type is not assignable" );
	  const ptrdiff_t _Num = __last - __first;
	  if (_Num)
	    __builtin_memmove(__result - _Num, __first, sizeof(_Tp) * _Num);
	  return __result - _Num;
	}
    };

  template<bool _IsMove, typename _BI1, typename _BI2>
    inline _BI2
    __copy_move_backward_a(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      typedef typename iterator_traits<_BI1>::value_type _ValueType1;
      typedef typename iterator_traits<_BI2>::value_type _ValueType2;
      typedef typename iterator_traits<_BI1>::iterator_category _Category;
      const bool __simple = (__is_trivial(_ValueType1)
	                     && __is_pointer<_BI1>::__value
	                     && __is_pointer<_BI2>::__value
			     && __are_same<_ValueType1, _ValueType2>::__value);

      return std::__copy_move_backward<_IsMove, __simple,
	                               _Category>::__copy_move_b(__first,
								 __last,
								 __result);
    }

  template<bool _IsMove, typename _BI1, typename _BI2>
    inline _BI2
    __copy_move_backward_a2(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      return _BI2(std::__copy_move_backward_a<_IsMove>
		  (std::__niter_base(__first), std::__niter_base(__last),
		   std::__niter_base(__result)));
    }

  
















 
  template<typename _BI1, typename _BI2>
    inline _BI2
    copy_backward(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      
      
      
      
      ;

      return (std::__copy_move_backward_a2<__is_move_iterator<_BI1>::__value>
	      (std::__miter_base(__first), std::__miter_base(__last),
	       __result));
    }

  
















 
  template<typename _BI1, typename _BI2>
    inline _BI2
    move_backward(_BI1 __first, _BI1 __last, _BI2 __result)
    {
      
      
      
      
      ;

      return std::__copy_move_backward_a2<true>(std::__miter_base(__first),
						std::__miter_base(__last),
						__result);
    }


  template<typename _ForwardIterator, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<!__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
 	     const _Tp& __value)
    {
      for (; __first != __last; ++__first)
	*__first = __value;
    }
    
  template<typename _ForwardIterator, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_scalar<_Tp>::__value, void>::__type
    __fill_a(_ForwardIterator __first, _ForwardIterator __last,
	     const _Tp& __value)
    {
      const _Tp __tmp = __value;
      for (; __first != __last; ++__first)
	*__first = __tmp;
    }

  
  template<typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_byte<_Tp>::__value, void>::__type
    __fill_a(_Tp* __first, _Tp* __last, const _Tp& __c)
    {
      const _Tp __tmp = __c;
      if (const size_t __len = __last - __first)
	__builtin_memset(__first, static_cast<unsigned char>(__tmp), __len);
    }

  










 
  template<typename _ForwardIterator, typename _Tp>
    inline void
    fill(_ForwardIterator __first, _ForwardIterator __last, const _Tp& __value)
    {
      
      
      ;

      std::__fill_a(std::__niter_base(__first), std::__niter_base(__last),
		    __value);
    }

  template<typename _OutputIterator, typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<!__is_scalar<_Tp>::__value, _OutputIterator>::__type
    __fill_n_a(_OutputIterator __first, _Size __n, const _Tp& __value)
    {
      for (__decltype(__n + 0) __niter = __n;
	   __niter > 0; --__niter, ++__first)
	*__first = __value;
      return __first;
    }

  template<typename _OutputIterator, typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_scalar<_Tp>::__value, _OutputIterator>::__type
    __fill_n_a(_OutputIterator __first, _Size __n, const _Tp& __value)
    {
      const _Tp __tmp = __value;
      for (__decltype(__n + 0) __niter = __n;
	   __niter > 0; --__niter, ++__first)
	*__first = __tmp;
      return __first;
    }

  template<typename _Size, typename _Tp>
    inline typename
    __gnu_cxx::__enable_if<__is_byte<_Tp>::__value, _Tp*>::__type
    __fill_n_a(_Tp* __first, _Size __n, const _Tp& __c)
    {
      std::__fill_a(__first, __first + __n, __c);
      return __first + __n;
    }

  













 
  template<typename _OI, typename _Size, typename _Tp>
    inline _OI
    fill_n(_OI __first, _Size __n, const _Tp& __value)
    {
      
      

      return _OI(std::__fill_n_a(std::__niter_base(__first), __n, __value));
    }

  template<bool _BoolType>
    struct __equal
    {
      template<typename _II1, typename _II2>
        static bool
        equal(_II1 __first1, _II1 __last1, _II2 __first2)
        {
	  for (; __first1 != __last1; ++__first1, (void)++__first2)
	    if (!(*__first1 == *__first2))
	      return false;
	  return true;
	}
    };

  template<>
    struct __equal<true>
    {
      template<typename _Tp>
        static bool
        equal(const _Tp* __first1, const _Tp* __last1, const _Tp* __first2)
        {
	  if (const size_t __len = (__last1 - __first1))
	    return !__builtin_memcmp(__first1, __first2, sizeof(_Tp) * __len);
	  return true;
	}
    };

  template<typename _II1, typename _II2>
    inline bool
    __equal_aux(_II1 __first1, _II1 __last1, _II2 __first2)
    {
      typedef typename iterator_traits<_II1>::value_type _ValueType1;
      typedef typename iterator_traits<_II2>::value_type _ValueType2;
      const bool __simple = ((__is_integer<_ValueType1>::__value
			      || __is_pointer<_ValueType1>::__value)
	                     && __is_pointer<_II1>::__value
	                     && __is_pointer<_II2>::__value
			     && __are_same<_ValueType1, _ValueType2>::__value);

      return std::__equal<__simple>::equal(__first1, __last1, __first2);
    }

  template<typename, typename>
    struct __lc_rai
    {
      template<typename _II1, typename _II2>
        static _II1
        __newlast1(_II1, _II1 __last1, _II2, _II2)
        { return __last1; }

      template<typename _II>
        static bool
        __cnd2(_II __first, _II __last)
        { return __first != __last; }
    };

  template<>
    struct __lc_rai<random_access_iterator_tag, random_access_iterator_tag>
    {
      template<typename _RAI1, typename _RAI2>
        static _RAI1
        __newlast1(_RAI1 __first1, _RAI1 __last1,
		   _RAI2 __first2, _RAI2 __last2)
        {
	  const typename iterator_traits<_RAI1>::difference_type
	    __diff1 = __last1 - __first1;
	  const typename iterator_traits<_RAI2>::difference_type
	    __diff2 = __last2 - __first2;
	  return __diff2 < __diff1 ? __first1 + __diff2 : __last1;
	}

      template<typename _RAI>
        static bool
        __cnd2(_RAI, _RAI)
        { return true; }
    };

  template<typename _II1, typename _II2, typename _Compare>
    bool
    __lexicographical_compare_impl(_II1 __first1, _II1 __last1,
				   _II2 __first2, _II2 __last2,
				   _Compare __comp)
    {
      typedef typename iterator_traits<_II1>::iterator_category _Category1;
      typedef typename iterator_traits<_II2>::iterator_category _Category2;
      typedef std::__lc_rai<_Category1, _Category2> __rai_type;

      __last1 = __rai_type::__newlast1(__first1, __last1, __first2, __last2);
      for (; __first1 != __last1 && __rai_type::__cnd2(__first2, __last2);
	   ++__first1, (void)++__first2)
	{
	  if (__comp(__first1, __first2))
	    return true;
	  if (__comp(__first2, __first1))
	    return false;
	}
      return __first1 == __last1 && __first2 != __last2;
    }

  template<bool _BoolType>
    struct __lexicographical_compare
    {
      template<typename _II1, typename _II2>
        static bool __lc(_II1, _II1, _II2, _II2);
    };

  template<bool _BoolType>
    template<typename _II1, typename _II2>
      bool
      __lexicographical_compare<_BoolType>::
      __lc(_II1 __first1, _II1 __last1, _II2 __first2, _II2 __last2)
      {
	return std::__lexicographical_compare_impl(__first1, __last1,
						   __first2, __last2,
					__gnu_cxx::__ops::__iter_less_iter());
      }

  template<>
    struct __lexicographical_compare<true>
    {
      template<typename _Tp, typename _Up>
        static bool
        __lc(const _Tp* __first1, const _Tp* __last1,
	     const _Up* __first2, const _Up* __last2)
	{
	  const size_t __len1 = __last1 - __first1;
	  const size_t __len2 = __last2 - __first2;
	  if (const size_t __len = std::min(__len1, __len2))
	    if (int __result = __builtin_memcmp(__first1, __first2, __len))
	      return __result < 0;
	  return __len1 < __len2;
	}
    };

  template<typename _II1, typename _II2>
    inline bool
    __lexicographical_compare_aux(_II1 __first1, _II1 __last1,
				  _II2 __first2, _II2 __last2)
    {
      typedef typename iterator_traits<_II1>::value_type _ValueType1;
      typedef typename iterator_traits<_II2>::value_type _ValueType2;
      const bool __simple =
	(__is_byte<_ValueType1>::__value && __is_byte<_ValueType2>::__value
	 && !__gnu_cxx::__numeric_traits<_ValueType1>::__is_signed
	 && !__gnu_cxx::__numeric_traits<_ValueType2>::__is_signed
	 && __is_pointer<_II1>::__value
	 && __is_pointer<_II2>::__value);

      return std::__lexicographical_compare<__simple>::__lc(__first1, __last1,
							    __first2, __last2);
    }

  template<typename _ForwardIterator, typename _Tp, typename _Compare>
    _ForwardIterator
    __lower_bound(_ForwardIterator __first, _ForwardIterator __last,
		  const _Tp& __val, _Compare __comp)
    {
      typedef typename iterator_traits<_ForwardIterator>::difference_type
	_DistanceType;

      _DistanceType __len = std::distance(__first, __last);

      while (__len > 0)
	{
	  _DistanceType __half = __len >> 1;
	  _ForwardIterator __middle = __first;
	  std::advance(__middle, __half);
	  if (__comp(__middle, __val))
	    {
	      __first = __middle;
	      ++__first;
	      __len = __len - __half - 1;
	    }
	  else
	    __len = __half;
	}
      return __first;
    }

  









 
  template<typename _ForwardIterator, typename _Tp>
    inline _ForwardIterator
    lower_bound(_ForwardIterator __first, _ForwardIterator __last,
		const _Tp& __val)
    {
      
      
      
      ;
      ;

      return std::__lower_bound(__first, __last, __val,
				__gnu_cxx::__ops::__iter_less_val());
    }

  
  
  inline constexpr int
  __lg(int __n)
  { return sizeof(int) * 8  - 1 - __builtin_clz(__n); }

  inline constexpr unsigned
  __lg(unsigned __n)
  { return sizeof(int) * 8  - 1 - __builtin_clz(__n); }

  inline constexpr long
  __lg(long __n)
  { return sizeof(long) * 8 - 1 - __builtin_clzl(__n); }

  inline constexpr unsigned long
  __lg(unsigned long __n)
  { return sizeof(long) * 8 - 1 - __builtin_clzl(__n); }

  inline constexpr long long
  __lg(long long __n)
  { return sizeof(long long) * 8 - 1 - __builtin_clzll(__n); }

  inline constexpr unsigned long long
  __lg(unsigned long long __n)
  { return sizeof(long long) * 8 - 1 - __builtin_clzll(__n); }





  










 
  template<typename _II1, typename _II2>
    inline bool
    equal(_II1 __first1, _II1 __last1, _II2 __first2)
    {
      
      
      
      
      ;

      return std::__equal_aux(std::__niter_base(__first1),
			      std::__niter_base(__last1),
			      std::__niter_base(__first2));
    }

  













 
  template<typename _IIter1, typename _IIter2, typename _BinaryPredicate>
    inline bool
    equal(_IIter1 __first1, _IIter1 __last1,
	  _IIter2 __first2, _BinaryPredicate __binary_pred)
    {
      
      
      
      ;

      for (; __first1 != __last1; ++__first1, (void)++__first2)
	if (!bool(__binary_pred(*__first1, *__first2)))
	  return false;
      return true;
    }



  











 
  template<typename _II1, typename _II2>
    inline bool
    equal(_II1 __first1, _II1 __last1, _II2 __first2, _II2 __last2)
    {
      
      
      
      
      ;
      ;

      using _RATag = random_access_iterator_tag;
      using _Cat1 = typename iterator_traits<_II1>::iterator_category;
      using _Cat2 = typename iterator_traits<_II2>::iterator_category;
      using _RAIters = __and_<is_same<_Cat1, _RATag>, is_same<_Cat2, _RATag>>;
      if (_RAIters())
	{
	  auto __d1 = std::distance(__first1, __last1);
	  auto __d2 = std::distance(__first2, __last2);
	  if (__d1 != __d2)
	    return false;
	  return std::equal(__first1, __last1, __first2);
	}

      for (; __first1 != __last1 && __first2 != __last2;
	  ++__first1, (void)++__first2)
	if (!(*__first1 == *__first2))
	  return false;
      return __first1 == __last1 && __first2 == __last2;
    }

  














 
  template<typename _IIter1, typename _IIter2, typename _BinaryPredicate>
    inline bool
    equal(_IIter1 __first1, _IIter1 __last1,
	  _IIter2 __first2, _IIter2 __last2, _BinaryPredicate __binary_pred)
    {
      
      
      
      ;
      ;

      using _RATag = random_access_iterator_tag;
      using _Cat1 = typename iterator_traits<_IIter1>::iterator_category;
      using _Cat2 = typename iterator_traits<_IIter2>::iterator_category;
      using _RAIters = __and_<is_same<_Cat1, _RATag>, is_same<_Cat2, _RATag>>;
      if (_RAIters())
	{
	  auto __d1 = std::distance(__first1, __last1);
	  auto __d2 = std::distance(__first2, __last2);
	  if (__d1 != __d2)
	    return false;
	  return std::equal(__first1, __last1, __first2,
				       __binary_pred);
	}

      for (; __first1 != __last1 && __first2 != __last2;
	  ++__first1, (void)++__first2)
	if (!bool(__binary_pred(*__first1, *__first2)))
	  return false;
      return __first1 == __last1 && __first2 == __last2;
    }

  













 
  template<typename _II1, typename _II2>
    inline bool
    lexicographical_compare(_II1 __first1, _II1 __last1,
			    _II2 __first2, _II2 __last2)
    {
      
      
      
      
      ;
      ;
      ;
      ;

      return std::__lexicographical_compare_aux(std::__niter_base(__first1),
						std::__niter_base(__last1),
						std::__niter_base(__first2),
						std::__niter_base(__last2));
    }

  











 
  template<typename _II1, typename _II2, typename _Compare>
    inline bool
    lexicographical_compare(_II1 __first1, _II1 __last1,
			    _II2 __first2, _II2 __last2, _Compare __comp)
    {
      
      
      
      ;
      ;
      ;
      ;

      return std::__lexicographical_compare_impl
	(__first1, __last1, __first2, __last2,
	 __gnu_cxx::__ops::__iter_comp_iter(__comp));
    }

  template<typename _InputIterator1, typename _InputIterator2,
	   typename _BinaryPredicate>
    pair<_InputIterator1, _InputIterator2>
    __mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	       _InputIterator2 __first2, _BinaryPredicate __binary_pred)
    {
      while (__first1 != __last1 && __binary_pred(__first1, __first2))
        {
	  ++__first1;
	  ++__first2;
        }
      return pair<_InputIterator1, _InputIterator2>(__first1, __first2);
    }

  











 
  template<typename _InputIterator1, typename _InputIterator2>
    inline pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2)
    {
      
      
      
      
      ;

      return std::__mismatch(__first1, __last1, __first2,
			     __gnu_cxx::__ops::__iter_equal_to_iter());
    }

  














 
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _BinaryPredicate>
    inline pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2, _BinaryPredicate __binary_pred)
    {
      
      
      
      ;

      return std::__mismatch(__first1, __last1, __first2,
	__gnu_cxx::__ops::__iter_comp_iter(__binary_pred));
    }


  template<typename _InputIterator1, typename _InputIterator2,
	   typename _BinaryPredicate>
    pair<_InputIterator1, _InputIterator2>
    __mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	       _InputIterator2 __first2, _InputIterator2 __last2,
	       _BinaryPredicate __binary_pred)
    {
      while (__first1 != __last1 && __first2 != __last2
	     && __binary_pred(__first1, __first2))
        {
	  ++__first1;
	  ++__first2;
        }
      return pair<_InputIterator1, _InputIterator2>(__first1, __first2);
    }

  












 
  template<typename _InputIterator1, typename _InputIterator2>
    inline pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2, _InputIterator2 __last2)
    {
      
      
      
      
      ;
      ;

      return std::__mismatch(__first1, __last1, __first2, __last2,
			     __gnu_cxx::__ops::__iter_equal_to_iter());
    }

  















 
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _BinaryPredicate>
    inline pair<_InputIterator1, _InputIterator2>
    mismatch(_InputIterator1 __first1, _InputIterator1 __last1,
	     _InputIterator2 __first2, _InputIterator2 __last2,
	     _BinaryPredicate __binary_pred)
    {
      
      
      
      ;
      ;

      return std::__mismatch(__first1, __last1, __first2, __last2,
			     __gnu_cxx::__ops::__iter_comp_iter(__binary_pred));
    }


} 

#pragma GCC visibility push(default)


extern "C++" {

namespace std
{
  
  class exception
  {
  public:
    exception() noexcept { }
    virtual ~exception()  noexcept;

    
 
    virtual const char*
    what() const  noexcept;
  };

  
 
  class bad_exception : public exception
  {
  public:
    bad_exception() noexcept { }

    
    
    virtual ~bad_exception()  noexcept;

    
    virtual const char*
    what() const  noexcept;
  };

  
  typedef void (*terminate_handler) ();

  
  typedef void (*unexpected_handler) ();

  
  terminate_handler set_terminate(terminate_handler) noexcept;

  
  terminate_handler get_terminate() noexcept;

  
 
  void terminate() noexcept __attribute__ ((__noreturn__));

  
  unexpected_handler set_unexpected(unexpected_handler) noexcept;

  
  unexpected_handler get_unexpected() noexcept;

  
 
  void unexpected() __attribute__ ((__noreturn__));

  









 
  bool uncaught_exception() noexcept __attribute__ ((__pure__));

  
  int uncaught_exceptions() noexcept __attribute__ ((__pure__));

  
} 

namespace __gnu_cxx
{

  void __verbose_terminate_handler();


} 

} 

#pragma GCC visibility pop

#pragma GCC visibility push(default)



extern "C++" {

namespace std 
{
  class type_info;

  


 
  namespace __exception_ptr
  {
    class exception_ptr;
  }

  using __exception_ptr::exception_ptr;

  


 
  exception_ptr current_exception() noexcept;

  
  void rethrow_exception(exception_ptr) __attribute__ ((__noreturn__));

  namespace __exception_ptr
  {
    using std::rethrow_exception;

    


 
    class exception_ptr
    {
      void* _M_exception_object;

      explicit exception_ptr(void* __e) noexcept;

      void _M_addref() noexcept;
      void _M_release() noexcept;

      void *_M_get() const noexcept __attribute__ ((__pure__));

      friend exception_ptr std::current_exception() noexcept;
      friend void std::rethrow_exception(exception_ptr);

    public:
      exception_ptr() noexcept;

      exception_ptr(const exception_ptr&) noexcept;

      exception_ptr(nullptr_t) noexcept
      : _M_exception_object(0)
      { }

      exception_ptr(exception_ptr&& __o) noexcept
      : _M_exception_object(__o._M_exception_object)
      { __o._M_exception_object = 0; }


      exception_ptr& 
      operator=(const exception_ptr&) noexcept;

      exception_ptr& 
      operator=(exception_ptr&& __o) noexcept
      {
        exception_ptr(static_cast<exception_ptr&&>(__o)).swap(*this);
        return *this;
      }

      ~exception_ptr() noexcept;

      void 
      swap(exception_ptr&) noexcept;


      explicit operator bool() const
      { return _M_exception_object; }

      friend bool 
      operator==(const exception_ptr&, const exception_ptr&)
	noexcept __attribute__ ((__pure__));

      const class std::type_info*
      __cxa_exception_type() const noexcept
	__attribute__ ((__pure__));
    };

    bool 
    operator==(const exception_ptr&, const exception_ptr&)
      noexcept __attribute__ ((__pure__));

    bool 
    operator!=(const exception_ptr&, const exception_ptr&)
      noexcept __attribute__ ((__pure__));

    inline void
    swap(exception_ptr& __lhs, exception_ptr& __rhs)
    { __lhs.swap(__rhs); }

  } 


  
  template<typename _Ex>
    exception_ptr 
    make_exception_ptr(_Ex __ex) noexcept
    {
      try
	{
	  throw __ex;
	}
      catch(...)
	{
	  return current_exception();
	}
    }

  
  
  
  
  template<typename _Ex>
    exception_ptr
    copy_exception(_Ex __ex) noexcept __attribute__ ((__deprecated__));

  template<typename _Ex>
    exception_ptr
    copy_exception(_Ex __ex) noexcept
    { return std::make_exception_ptr<_Ex>(__ex); }

  
} 

} 

#pragma GCC visibility pop

#pragma GCC visibility push(default)

extern "C++" {

namespace std
{
  
  class nested_exception
  {
    exception_ptr _M_ptr;

  public:
    nested_exception() noexcept : _M_ptr(current_exception()) { }

    nested_exception(const nested_exception&) noexcept = default;

    nested_exception& operator=(const nested_exception&) noexcept = default;

    virtual ~nested_exception() noexcept;

    [[noreturn]]
    void
    rethrow_nested() const
    {
      if (_M_ptr)
	rethrow_exception(_M_ptr);
      std::terminate();
    }

    exception_ptr
    nested_ptr() const noexcept
    { return _M_ptr; }
  };

  template<typename _Except>
    struct _Nested_exception : public _Except, public nested_exception
    {
      explicit _Nested_exception(const _Except& __ex)
      : _Except(__ex)
      { }

      explicit _Nested_exception(_Except&& __ex)
      : _Except(static_cast<_Except&&>(__ex))
      { }
    };

  template<typename _Tp,
	   bool __with_nested = !__is_base_of(nested_exception, _Tp)>
    struct _Throw_with_nested_impl
    {
      template<typename _Up>
	static void _S_throw(_Up&& __t)
	{ throw _Nested_exception<_Tp>{static_cast<_Up&&>(__t)}; }
    };

  template<typename _Tp>
    struct _Throw_with_nested_impl<_Tp, false>
    {
      template<typename _Up>
	static void _S_throw(_Up&& __t)
	{ throw static_cast<_Up&&>(__t); }
    };

  template<typename _Tp, bool = __is_class(_Tp) && !__is_final(_Tp)>
    struct _Throw_with_nested_helper : _Throw_with_nested_impl<_Tp>
    { };

  template<typename _Tp>
    struct _Throw_with_nested_helper<_Tp, false>
    : _Throw_with_nested_impl<_Tp, false>
    { };

  template<typename _Tp>
    struct _Throw_with_nested_helper<_Tp&, false>
    : _Throw_with_nested_helper<_Tp>
    { };

  template<typename _Tp>
    struct _Throw_with_nested_helper<_Tp&&, false>
    : _Throw_with_nested_helper<_Tp>
    { };

  
  
  template<typename _Tp>
    [[noreturn]]
    inline void
    throw_with_nested(_Tp&& __t)
    {
      _Throw_with_nested_helper<_Tp>::_S_throw(static_cast<_Tp&&>(__t));
    }

  template<typename _Tp, bool = __is_polymorphic(_Tp)>
    struct _Rethrow_if_nested_impl
    {
      static void _S_rethrow(const _Tp& __t)
      {
	if (auto __tp =
            dynamic_cast<const nested_exception*>(std::__addressof(__t)))
	  __tp->rethrow_nested();
      }
    };

  template<typename _Tp>
    struct _Rethrow_if_nested_impl<_Tp, false>
    {
      static void _S_rethrow(const _Tp&) { }
    };

  
  template<typename _Ex>
    inline void
    rethrow_if_nested(const _Ex& __ex)
    {
      _Rethrow_if_nested_impl<_Ex>::_S_rethrow(__ex);
    }

  
} 

} 


#pragma GCC visibility pop



#pragma GCC visibility push(default)

extern "C++" {

namespace std 
{
  




 
  class bad_alloc : public exception 
  {
  public:
    bad_alloc() throw() { }

    
    
    virtual ~bad_alloc() throw();

    
    virtual const char* what() const throw();
  };

  class bad_array_new_length : public bad_alloc
  {
  public:
    bad_array_new_length() throw() { };

    
    
    virtual ~bad_array_new_length() throw();

    
    virtual const char* what() const throw();
  };

  struct nothrow_t
  {
    explicit nothrow_t() = default;
  };

  extern const nothrow_t nothrow;

  
 
  typedef void (*new_handler)();

  
  
  new_handler set_new_handler(new_handler) throw();

  
  new_handler get_new_handler() noexcept;
} 











 
void* operator new(std::size_t) 
  __attribute__((__externally_visible__));
void* operator new[](std::size_t) 
  __attribute__((__externally_visible__));
void operator delete(void*) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*) noexcept
  __attribute__((__externally_visible__));
void operator delete(void*, std::size_t) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*, std::size_t) noexcept
  __attribute__((__externally_visible__));
void* operator new(std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void* operator new[](std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete(void*, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete(void*, std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));
void operator delete[](void*, std::size_t, const std::nothrow_t&) noexcept
  __attribute__((__externally_visible__));


inline void* operator new(std::size_t, void* __p) noexcept
{ return __p; }
inline void* operator new[](std::size_t, void* __p) noexcept
{ return __p; }


inline void operator delete  (void*, void*) noexcept { }
inline void operator delete[](void*, void*) noexcept { }

} 

#pragma GCC visibility pop


namespace __gnu_cxx 
{


  using std::size_t;
  using std::ptrdiff_t;

  








 
  template<typename _Tp>
    class new_allocator
    {
    public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };

      
      
      typedef std::true_type propagate_on_container_move_assignment;

      new_allocator() noexcept { }

      new_allocator(const new_allocator&) noexcept { }

      template<typename _Tp1>
        new_allocator(const new_allocator<_Tp1>&) noexcept { }

      ~new_allocator() noexcept { }

      pointer
      address(reference __x) const noexcept
      { return std::__addressof(__x); }

      const_pointer
      address(const_reference __x) const noexcept
      { return std::__addressof(__x); }

      
      
      pointer
      allocate(size_type __n, const void* = 0)
      { 
	if (__n > this->max_size())
	  std::__throw_bad_alloc();

	return static_cast<_Tp*>(::operator new(__n * sizeof(_Tp)));
      }

      
      void
      deallocate(pointer __p, size_type)
      { ::operator delete(__p); }

      size_type
      max_size() const noexcept
      { return size_t(-1) / sizeof(_Tp); }

      template<typename _Up, typename... _Args>
        void
        construct(_Up* __p, _Args&&... __args)
	{ ::new((void *)__p) _Up(std::forward<_Args>(__args)...); }

      template<typename _Up>
        void 
        destroy(_Up* __p) { __p->~_Up(); }
    };

  template<typename _Tp>
    inline bool
    operator==(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return true; }
  
  template<typename _Tp>
    inline bool
    operator!=(const new_allocator<_Tp>&, const new_allocator<_Tp>&)
    { return false; }


} 


namespace std
{
  
  template<typename _Tp>
    using __allocator_base = __gnu_cxx::new_allocator<_Tp>;
}

namespace std 
{

  template<typename>
    class allocator;

  template<>
    class allocator<void>;

  
  template<typename, typename>
    struct uses_allocator;

} 


namespace std 
{
  
  template<>
    class allocator<void>
    {
    public:
      typedef size_t      size_type;
      typedef ptrdiff_t   difference_type;
      typedef void*       pointer;
      typedef const void* const_pointer;
      typedef void        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      
      
      typedef true_type propagate_on_container_move_assignment;

      typedef true_type is_always_equal;
    };

 
  template<typename _Tp>
    class allocator: public __allocator_base<_Tp>
    {
   public:
      typedef size_t     size_type;
      typedef ptrdiff_t  difference_type;
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;
      typedef _Tp&       reference;
      typedef const _Tp& const_reference;
      typedef _Tp        value_type;

      template<typename _Tp1>
        struct rebind
        { typedef allocator<_Tp1> other; };

      
      
      typedef true_type propagate_on_container_move_assignment;

      allocator() throw() { }

      allocator(const allocator& __a) throw()
      : __allocator_base<_Tp>(__a) { }

      template<typename _Tp1>
        allocator(const allocator<_Tp1>&) throw() { }

      ~allocator() throw() { }

      
    };

  template<typename _T1, typename _T2>
    inline bool
    operator==(const allocator<_T1>&, const allocator<_T2>&)
    noexcept
    { return true; }

  template<typename _Tp>
    inline bool
    operator==(const allocator<_Tp>&, const allocator<_Tp>&)
    noexcept
    { return true; }

  template<typename _T1, typename _T2>
    inline bool
    operator!=(const allocator<_T1>&, const allocator<_T2>&)
    noexcept
    { return false; }

  template<typename _Tp>
    inline bool
    operator!=(const allocator<_Tp>&, const allocator<_Tp>&)
    noexcept
    { return false; }

  

  
  
  extern template class allocator<char>;
  extern template class allocator<wchar_t>;

  

  
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_swap
    { static void _S_do_it(_Alloc&, _Alloc&) noexcept { } };

  template<typename _Alloc>
    struct __alloc_swap<_Alloc, false>
    {
      static void
      _S_do_it(_Alloc& __one, _Alloc& __two) noexcept
      {
	
	if (__one != __two)
	  swap(__one, __two);
      }
    };

  
  template<typename _Alloc, bool = __is_empty(_Alloc)>
    struct __alloc_neq
    {
      static bool
      _S_do_it(const _Alloc&, const _Alloc&)
      { return false; }
    };

  template<typename _Alloc>
    struct __alloc_neq<_Alloc, false>
    {
      static bool
      _S_do_it(const _Alloc& __one, const _Alloc& __two)
      { return __one != __two; }
    };

  template<typename _Tp, bool
    = __or_<is_copy_constructible<typename _Tp::value_type>,
            is_nothrow_move_constructible<typename _Tp::value_type>>::value>
    struct __shrink_to_fit_aux
    { static bool _S_do_it(_Tp&) noexcept { return false; } };

  template<typename _Tp>
    struct __shrink_to_fit_aux<_Tp, true>
    {
      static bool
      _S_do_it(_Tp& __c) noexcept
      {
	try
	  {
	    _Tp(__make_move_if_noexcept_iterator(__c.begin()),
		__make_move_if_noexcept_iterator(__c.end()),
		__c.get_allocator()).swap(__c);
	    return true;
	  }
	catch(...)
	  { return false; }
      }
    };

} 

namespace std 
{


  struct __allocator_traits_base
  {
    template<typename _Alloc, typename _Up>
      using __rebind = typename _Alloc::template rebind<_Up>::other;

  protected:
    template<typename _Tp>
      using __pointer = typename _Tp::pointer;
    template<typename _Tp>
      using __c_pointer = typename _Tp::const_pointer;
    template<typename _Tp>
      using __v_pointer = typename _Tp::void_pointer;
    template<typename _Tp>
      using __cv_pointer = typename _Tp::const_void_pointer;
    template<typename _Tp>
      using __diff_type = typename _Tp::difference_type;
    template<typename _Tp>
      using __size_type = typename _Tp::size_type;
    template<typename _Tp>
      using __pocca = typename _Tp::propagate_on_container_copy_assignment;
    template<typename _Tp>
      using __pocma = typename _Tp::propagate_on_container_move_assignment;
    template<typename _Tp>
      using __pocs = typename _Tp::propagate_on_container_swap;
    template<typename _Tp>
      using __equal = typename _Tp::is_always_equal;
  };

  template<typename _Alloc, typename _Up>
    using __alloc_rebind = __detected_or_t_<__replace_first_arg_t,
					    __allocator_traits_base::__rebind,
					    _Alloc, _Up>;

  


 
  template<typename _Alloc>
    struct allocator_traits : __allocator_traits_base
    {
      
      typedef _Alloc allocator_type;
      
      typedef typename _Alloc::value_type value_type;

      



 
      using pointer = __detected_or_t<value_type*, __pointer, _Alloc>;

      




 
      using const_pointer
	= __detected_or_t<__ptr_rebind<pointer, const value_type>,
			  __c_pointer, _Alloc>;

      




 
      using void_pointer
	= __detected_or_t<__ptr_rebind<pointer, void>, __v_pointer, _Alloc>;

      




 
      using const_void_pointer
	= __detected_or_t<__ptr_rebind<pointer, const void>, __cv_pointer,
			  _Alloc>;

      




 
      using difference_type
	= __detected_or_t<typename pointer_traits<pointer>::difference_type,
			  __diff_type, _Alloc>;

      




 
      using size_type
	= __detected_or_t<typename make_unsigned<difference_type>::type,
			  __size_type, _Alloc>;

      




 
      using propagate_on_container_copy_assignment
	= __detected_or_t<false_type, __pocca, _Alloc>;

      




 
      using propagate_on_container_move_assignment
	= __detected_or_t<false_type, __pocma, _Alloc>;

      




 
      using propagate_on_container_swap
	= __detected_or_t<false_type, __pocs, _Alloc>;

      




 
      using is_always_equal
	= __detected_or_t<typename is_empty<_Alloc>::type, __equal, _Alloc>;

      template<typename _Tp>
	using rebind_alloc = __alloc_rebind<_Alloc, _Tp>;
      template<typename _Tp>
	using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;

      static_assert(!is_same<rebind_alloc<value_type>, __undefined>::value,
	  "allocator defines rebind or is like Alloc<T, Args>");

    private:
      template<typename _Alloc2>
	static auto
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer __hint, int)
	-> decltype(__a.allocate(__n, __hint))
	{ return __a.allocate(__n, __hint); }

      template<typename _Alloc2>
	static pointer
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer, ...)
	{ return __a.allocate(__n); }

      template<typename _Tp, typename... _Args>
	struct __construct_helper
	{
	  template<typename _Alloc2,
	    typename = decltype(std::declval<_Alloc2*>()->construct(
		  std::declval<_Tp*>(), std::declval<_Args>()...))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  using type = decltype(__test<_Alloc>(0));
	};

      template<typename _Tp, typename... _Args>
	using __has_construct
	  = typename __construct_helper<_Tp, _Args...>::type;

      template<typename _Tp, typename... _Args>
	static _Require<__has_construct<_Tp, _Args...>>
	_S_construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	{ __a.construct(__p, std::forward<_Args>(__args)...); }

      template<typename _Tp, typename... _Args>
	static
	_Require<__and_<__not_<__has_construct<_Tp, _Args...>>,
			       is_constructible<_Tp, _Args...>>>
	_S_construct(_Alloc&, _Tp* __p, _Args&&... __args)
	{ ::new((void*)__p) _Tp(std::forward<_Args>(__args)...); }

      template<typename _Alloc2, typename _Tp>
	static auto
	_S_destroy(_Alloc2& __a, _Tp* __p, int)
	-> decltype(__a.destroy(__p))
	{ __a.destroy(__p); }

      template<typename _Alloc2, typename _Tp>
	static void
	_S_destroy(_Alloc2&, _Tp* __p, ...)
	{ __p->~_Tp(); }

      template<typename _Alloc2>
	static auto
	_S_max_size(_Alloc2& __a, int)
	-> decltype(__a.max_size())
	{ return __a.max_size(); }

      template<typename _Alloc2>
	static size_type
	_S_max_size(_Alloc2&, ...)
	{
	  
	  
	  return __gnu_cxx::__numeric_traits<size_type>::__max
	    / sizeof(value_type);
	}

      template<typename _Alloc2>
	static auto
	_S_select(_Alloc2& __a, int)
	-> decltype(__a.select_on_container_copy_construction())
	{ return __a.select_on_container_copy_construction(); }

      template<typename _Alloc2>
	static _Alloc2
	_S_select(_Alloc2& __a, ...)
	{ return __a; }

    public:

      





 
      static pointer
      allocate(_Alloc& __a, size_type __n)
      { return __a.allocate(__n); }

      









 
      static pointer
      allocate(_Alloc& __a, size_type __n, const_void_pointer __hint)
      { return _S_allocate(__a, __n, __hint, 0); }

      






 
      static void
      deallocate(_Alloc& __a, pointer __p, size_type __n)
      { __a.deallocate(__p, __n); }

      









 
      template<typename _Tp, typename... _Args>
	static auto construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	-> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...))
	{ _S_construct(__a, __p, std::forward<_Args>(__args)...); }

      






 
      template<typename _Tp>
	static void destroy(_Alloc& __a, _Tp* __p)
	{ _S_destroy(__a, __p, 0); }

      






 
      static size_type max_size(const _Alloc& __a) noexcept
      { return _S_max_size(__a, 0); }

      






 
      static _Alloc
      select_on_container_copy_construction(const _Alloc& __rhs)
      { return _S_select(__rhs, 0); }
    };

  
  template<typename _Tp>
    struct allocator_traits<allocator<_Tp>>
    {
      
      using allocator_type = allocator<_Tp>;
      
      using value_type = _Tp;

      
      using pointer = _Tp*;

      
      using const_pointer = const _Tp*;

      
      using void_pointer = void*;

      
      using const_void_pointer = const void*;

      
      using difference_type = std::ptrdiff_t;

      
      using size_type = std::size_t;

      
      using propagate_on_container_copy_assignment = false_type;

      
      using propagate_on_container_move_assignment = true_type;

      
      using propagate_on_container_swap = false_type;

      
      using is_always_equal = true_type;

      template<typename _Up>
	using rebind_alloc = allocator<_Up>;

      template<typename _Up>
	using rebind_traits = allocator_traits<allocator<_Up>>;

      





 
      static pointer
      allocate(allocator_type& __a, size_type __n)
      { return __a.allocate(__n); }

      








 
      static pointer
      allocate(allocator_type& __a, size_type __n, const_void_pointer __hint)
      { return __a.allocate(__n, __hint); }

      






 
      static void
      deallocate(allocator_type& __a, pointer __p, size_type __n)
      { __a.deallocate(__p, __n); }

      






 
      template<typename _Up, typename... _Args>
	static void
	construct(allocator_type& __a, _Up* __p, _Args&&... __args)
	{ __a.construct(__p, std::forward<_Args>(__args)...); }

      





 
      template<typename _Up>
	static void
	destroy(allocator_type& __a, _Up* __p)
	{ __a.destroy(__p); }

      



 
      static size_type
      max_size(const allocator_type& __a) noexcept
      { return __a.max_size(); }

      



 
      static allocator_type
      select_on_container_copy_construction(const allocator_type& __rhs)
      { return __rhs; }
    };


  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc& __one, const _Alloc& __two, true_type)
    { __one = __two; }

  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc&, const _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_copy(_Alloc& __one, const _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_copy_assignment __pocca;
      __do_alloc_on_copy(__one, __two, __pocca());
    }

  template<typename _Alloc>
    inline _Alloc __alloc_on_copy(const _Alloc& __a)
    {
      typedef allocator_traits<_Alloc> __traits;
      return __traits::select_on_container_copy_construction(__a);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc& __one, _Alloc& __two, true_type)
    { __one = std::move(__two); }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_move(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_move_assignment __pocma;
      __do_alloc_on_move(__one, __two, __pocma());
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc& __one, _Alloc& __two, true_type)
    {
      using std::swap;
      swap(__one, __two);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_swap(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_swap __pocs;
      __do_alloc_on_swap(__one, __two, __pocs());
    }

  template<typename _Alloc>
    class __is_copy_insertable_impl
    {
      typedef allocator_traits<_Alloc> _Traits;

      template<typename _Up, typename
	       = decltype(_Traits::construct(std::declval<_Alloc&>(),
					     std::declval<_Up*>(),
					     std::declval<const _Up&>()))>
	static true_type
	_M_select(int);

      template<typename _Up>
	static false_type
	_M_select(...);

    public:
      typedef decltype(_M_select<typename _Alloc::value_type>(0)) type;
    };

  
  template<typename _Alloc>
    struct __is_copy_insertable
    : __is_copy_insertable_impl<_Alloc>::type
    { };

  
  template<typename _Tp>
    struct __is_copy_insertable<allocator<_Tp>>
    : is_copy_constructible<_Tp>
    { };


} 


namespace __gnu_cxx 
{





 
template<typename _Alloc>
  struct __alloc_traits
  : std::allocator_traits<_Alloc>
  {
    typedef _Alloc allocator_type;
    typedef std::allocator_traits<_Alloc>           _Base_type;
    typedef typename _Base_type::value_type         value_type;
    typedef typename _Base_type::pointer            pointer;
    typedef typename _Base_type::const_pointer      const_pointer;
    typedef typename _Base_type::size_type          size_type;
    typedef typename _Base_type::difference_type    difference_type;
    
    typedef value_type&                             reference;
    typedef const value_type&                       const_reference;
    using _Base_type::allocate;
    using _Base_type::deallocate;
    using _Base_type::construct;
    using _Base_type::destroy;
    using _Base_type::max_size;

  private:
    template<typename _Ptr>
      using __is_custom_pointer
	= std::__and_<std::is_same<pointer, _Ptr>,
		      std::__not_<std::is_pointer<_Ptr>>>;

  public:
    
    template<typename _Ptr, typename... _Args>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      construct(_Alloc& __a, _Ptr __p, _Args&&... __args)
      {
	_Base_type::construct(__a, std::addressof(*__p),
			      std::forward<_Args>(__args)...);
      }

    
    template<typename _Ptr>
      static typename std::enable_if<__is_custom_pointer<_Ptr>::value>::type
      destroy(_Alloc& __a, _Ptr __p)
      { _Base_type::destroy(__a, std::addressof(*__p)); }

    static _Alloc _S_select_on_copy(const _Alloc& __a)
    { return _Base_type::select_on_container_copy_construction(__a); }

    static void _S_on_swap(_Alloc& __a, _Alloc& __b)
    { std::__alloc_on_swap(__a, __b); }

    static constexpr bool _S_propagate_on_copy_assign()
    { return _Base_type::propagate_on_container_copy_assignment::value; }

    static constexpr bool _S_propagate_on_move_assign()
    { return _Base_type::propagate_on_container_move_assignment::value; }

    static constexpr bool _S_propagate_on_swap()
    { return _Base_type::propagate_on_container_swap::value; }

    static constexpr bool _S_always_equal()
    { return _Base_type::is_always_equal::value; }

    static constexpr bool _S_nothrow_move()
    { return _S_propagate_on_move_assign() || _S_always_equal(); }

    template<typename _Tp>
      struct rebind
      { typedef typename _Base_type::template rebind_alloc<_Tp> other; };
  };


} 


namespace std 
{


  


 
  template<typename _T1, typename... _Args>
    inline void
    _Construct(_T1* __p, _Args&&... __args)
    { ::new(static_cast<void*>(__p)) _T1(std::forward<_Args>(__args)...); }

  

 
  template<typename _Tp>
    inline void
    _Destroy(_Tp* __pointer)
    { __pointer->~_Tp(); }

  template<bool>
    struct _Destroy_aux
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator __first, _ForwardIterator __last)
	{
	  for (; __first != __last; ++__first)
	    std::_Destroy(std::__addressof(*__first));
	}
    };

  template<>
    struct _Destroy_aux<true>
    {
      template<typename _ForwardIterator>
        static void
        __destroy(_ForwardIterator, _ForwardIterator) { }
    };

  



 
  template<typename _ForwardIterator>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
                       _Value_type;
      std::_Destroy_aux<__has_trivial_destructor(_Value_type)>::
	__destroy(__first, __last);
    }

  



 

  template<typename _ForwardIterator, typename _Allocator>
    void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     _Allocator& __alloc)
    {
      typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
      for (; __first != __last; ++__first)
	__traits::destroy(__alloc, std::__addressof(*__first));
    }

  template<typename _ForwardIterator, typename _Tp>
    inline void
    _Destroy(_ForwardIterator __first, _ForwardIterator __last,
	     allocator<_Tp>&)
    {
      _Destroy(__first, __last);
    }


} 


















































 




 


namespace std 
{


  template<bool _TrivialValueTypes>
    struct __uninitialized_copy
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        __uninit_copy(_InputIterator __first, _InputIterator __last,
		      _ForwardIterator __result)
        {
	  _ForwardIterator __cur = __result;
	  try
	    {
	      for (; __first != __last; ++__first, (void)++__cur)
		std::_Construct(std::__addressof(*__cur), *__first);
	      return __cur;
	    }
	  catch(...)
	    {
	      std::_Destroy(__result, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_copy<true>
    {
      template<typename _InputIterator, typename _ForwardIterator>
        static _ForwardIterator
        __uninit_copy(_InputIterator __first, _InputIterator __last,
		      _ForwardIterator __result)
        { return std::copy(__first, __last, __result); }
    };

  







 
  template<typename _InputIterator, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy(_InputIterator __first, _InputIterator __last,
		       _ForwardIterator __result)
    {
      typedef typename iterator_traits<_InputIterator>::value_type
	_ValueType1;
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType2;
      
      typedef typename iterator_traits<_InputIterator>::reference _RefType1;
      typedef typename iterator_traits<_ForwardIterator>::reference _RefType2;
      const bool __assignable = is_assignable<_RefType2, _RefType1>::value;

      return std::__uninitialized_copy<__is_trivial(_ValueType1)
				       && __is_trivial(_ValueType2)
				       && __assignable>::
	__uninit_copy(__first, __last, __result);
    }


  template<bool _TrivialValueType>
    struct __uninitialized_fill
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __uninit_fill(_ForwardIterator __first, _ForwardIterator __last,
		      const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __cur != __last; ++__cur)
		std::_Construct(std::__addressof(*__cur), __x);
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_fill<true>
    {
      template<typename _ForwardIterator, typename _Tp>
        static void
        __uninit_fill(_ForwardIterator __first, _ForwardIterator __last,
		      const _Tp& __x)
        { std::fill(__first, __last, __x); }
    };

  







 
  template<typename _ForwardIterator, typename _Tp>
    inline void
    uninitialized_fill(_ForwardIterator __first, _ForwardIterator __last,
		       const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      
      const bool __assignable = is_copy_assignable<_ValueType>::value;

      std::__uninitialized_fill<__is_trivial(_ValueType) && __assignable>::
	__uninit_fill(__first, __last, __x);
    }


  template<bool _TrivialValueType>
    struct __uninitialized_fill_n
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static _ForwardIterator
        __uninit_fill_n(_ForwardIterator __first, _Size __n,
			const _Tp& __x)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __n > 0; --__n, ++__cur)
		std::_Construct(std::__addressof(*__cur), __x);
	      return __cur;
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_fill_n<true>
    {
      template<typename _ForwardIterator, typename _Size, typename _Tp>
        static _ForwardIterator
        __uninit_fill_n(_ForwardIterator __first, _Size __n,
			const _Tp& __x)
        { return std::fill_n(__first, __n, __x); }
    };

   
   
  







 
  template<typename _ForwardIterator, typename _Size, typename _Tp>
    inline _ForwardIterator
    uninitialized_fill_n(_ForwardIterator __first, _Size __n, const _Tp& __x)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      
      const bool __assignable = is_copy_assignable<_ValueType>::value;
      return __uninitialized_fill_n<__is_trivial(_ValueType) && __assignable>::
	__uninit_fill_n(__first, __n, __x);
    }

  
  
  
  
  

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __result;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __first != __last; ++__first, (void)++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), *__first);
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__result, __cur, __alloc);
	  throw;
	}
    }

  template<typename _InputIterator, typename _ForwardIterator, typename _Tp>
    inline _ForwardIterator
    __uninitialized_copy_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, allocator<_Tp>&)
    { return std::uninitialized_copy(__first, __last, __result); }

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_a(_InputIterator __first, _InputIterator __last,
			   _ForwardIterator __result, _Allocator& __alloc)
    {
      return std::__uninitialized_copy_a(std::make_move_iterator(__first),
					 std::make_move_iterator(__last),
					 __result, __alloc);
    }

  template<typename _InputIterator, typename _ForwardIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_if_noexcept_a(_InputIterator __first,
				       _InputIterator __last,
				       _ForwardIterator __result,
				       _Allocator& __alloc)
    {
      return std::__uninitialized_copy_a
	(std::__make_move_if_noexcept_iterator(__first),
	 std::__make_move_if_noexcept_iterator(__last), __result, __alloc);
    }

  template<typename _ForwardIterator, typename _Tp, typename _Allocator>
    void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __cur != __last; ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), __x);
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Tp, typename _Tp2>
    inline void
    __uninitialized_fill_a(_ForwardIterator __first, _ForwardIterator __last,
			   const _Tp& __x, allocator<_Tp2>&)
    { std::uninitialized_fill(__first, __last, __x); }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Allocator>
    _ForwardIterator
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __n > 0; --__n, ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur), __x);
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Size, typename _Tp,
	   typename _Tp2>
    inline _ForwardIterator
    __uninitialized_fill_n_a(_ForwardIterator __first, _Size __n, 
			     const _Tp& __x, allocator<_Tp2>&)
    { return std::uninitialized_fill_n(__first, __n, __x); }


  
  
  
  

  
  
  
  
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_copy_move(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_copy_a(__first1, __last1,
							   __result,
							   __alloc);
      try
	{
	  return std::__uninitialized_move_a(__first2, __last2, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }

  
  
  
  
  template<typename _InputIterator1, typename _InputIterator2,
	   typename _ForwardIterator, typename _Allocator>
    inline _ForwardIterator
    __uninitialized_move_copy(_InputIterator1 __first1,
			      _InputIterator1 __last1,
			      _InputIterator2 __first2,
			      _InputIterator2 __last2,
			      _ForwardIterator __result,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid = std::__uninitialized_move_a(__first1, __last1,
							   __result,
							   __alloc);
      try
	{
	  return std::__uninitialized_copy_a(__first2, __last2, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }
  
  
  
  
  template<typename _ForwardIterator, typename _Tp, typename _InputIterator,
	   typename _Allocator>
    inline _ForwardIterator
    __uninitialized_fill_move(_ForwardIterator __result, _ForwardIterator __mid,
			      const _Tp& __x, _InputIterator __first,
			      _InputIterator __last, _Allocator& __alloc)
    {
      std::__uninitialized_fill_a(__result, __mid, __x, __alloc);
      try
	{
	  return std::__uninitialized_move_a(__first, __last, __mid, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__result, __mid, __alloc);
	  throw;
	}
    }

  
  
  
  template<typename _InputIterator, typename _ForwardIterator, typename _Tp,
	   typename _Allocator>
    inline void
    __uninitialized_move_fill(_InputIterator __first1, _InputIterator __last1,
			      _ForwardIterator __first2,
			      _ForwardIterator __last2, const _Tp& __x,
			      _Allocator& __alloc)
    {
      _ForwardIterator __mid2 = std::__uninitialized_move_a(__first1, __last1,
							    __first2,
							    __alloc);
      try
	{
	  std::__uninitialized_fill_a(__mid2, __last2, __x, __alloc);
	}
      catch(...)
	{
	  std::_Destroy(__first2, __mid2, __alloc);
	  throw;
	}
    }

  
  

  template<bool _TrivialValueType>
    struct __uninitialized_default_1
    {
      template<typename _ForwardIterator>
        static void
        __uninit_default(_ForwardIterator __first, _ForwardIterator __last)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __cur != __last; ++__cur)
		std::_Construct(std::__addressof(*__cur));
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_default_1<true>
    {
      template<typename _ForwardIterator>
        static void
        __uninit_default(_ForwardIterator __first, _ForwardIterator __last)
        {
	  typedef typename iterator_traits<_ForwardIterator>::value_type
	    _ValueType;

	  std::fill(__first, __last, _ValueType());
	}
    };

  template<bool _TrivialValueType>
    struct __uninitialized_default_n_1
    {
      template<typename _ForwardIterator, typename _Size>
        static _ForwardIterator
        __uninit_default_n(_ForwardIterator __first, _Size __n)
        {
	  _ForwardIterator __cur = __first;
	  try
	    {
	      for (; __n > 0; --__n, ++__cur)
		std::_Construct(std::__addressof(*__cur));
	      return __cur;
	    }
	  catch(...)
	    {
	      std::_Destroy(__first, __cur);
	      throw;
	    }
	}
    };

  template<>
    struct __uninitialized_default_n_1<true>
    {
      template<typename _ForwardIterator, typename _Size>
        static _ForwardIterator
        __uninit_default_n(_ForwardIterator __first, _Size __n)
        {
	  typedef typename iterator_traits<_ForwardIterator>::value_type
	    _ValueType;

	  return std::fill_n(__first, __n, _ValueType());
	}
    };

  
  
  
  template<typename _ForwardIterator>
    inline void
    __uninitialized_default(_ForwardIterator __first,
			    _ForwardIterator __last)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      
      const bool __assignable = is_copy_assignable<_ValueType>::value;

      std::__uninitialized_default_1<__is_trivial(_ValueType)
				     && __assignable>::
	__uninit_default(__first, __last);
    }

  
  
  template<typename _ForwardIterator, typename _Size>
    inline _ForwardIterator
    __uninitialized_default_n(_ForwardIterator __first, _Size __n)
    {
      typedef typename iterator_traits<_ForwardIterator>::value_type
	_ValueType;
      
      const bool __assignable = is_copy_assignable<_ValueType>::value;

      return __uninitialized_default_n_1<__is_trivial(_ValueType)
				       && __assignable>::
	__uninit_default_n(__first, __n);
    }


  
  
  
  template<typename _ForwardIterator, typename _Allocator>
    void
    __uninitialized_default_a(_ForwardIterator __first,
			      _ForwardIterator __last,
			      _Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __cur != __last; ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur));
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Tp>
    inline void
    __uninitialized_default_a(_ForwardIterator __first,
			      _ForwardIterator __last,
			      allocator<_Tp>&)
    { std::__uninitialized_default(__first, __last); }


  
  
  
  template<typename _ForwardIterator, typename _Size, typename _Allocator>
    _ForwardIterator
    __uninitialized_default_n_a(_ForwardIterator __first, _Size __n, 
				_Allocator& __alloc)
    {
      _ForwardIterator __cur = __first;
      try
	{
	  typedef __gnu_cxx::__alloc_traits<_Allocator> __traits;
	  for (; __n > 0; --__n, ++__cur)
	    __traits::construct(__alloc, std::__addressof(*__cur));
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__first, __cur, __alloc);
	  throw;
	}
    }

  template<typename _ForwardIterator, typename _Size, typename _Tp>
    inline _ForwardIterator
    __uninitialized_default_n_a(_ForwardIterator __first, _Size __n, 
				allocator<_Tp>&)
    { return std::__uninitialized_default_n(__first, __n); }


  template<typename _InputIterator, typename _Size,
	   typename _ForwardIterator>
    _ForwardIterator
    __uninitialized_copy_n(_InputIterator __first, _Size __n,
			   _ForwardIterator __result, input_iterator_tag)
    {
      _ForwardIterator __cur = __result;
      try
	{
	  for (; __n > 0; --__n, ++__first, ++__cur)
	    std::_Construct(std::__addressof(*__cur), *__first);
	  return __cur;
	}
      catch(...)
	{
	  std::_Destroy(__result, __cur);
	  throw;
	}
    }

  template<typename _RandomAccessIterator, typename _Size,
	   typename _ForwardIterator>
    inline _ForwardIterator
    __uninitialized_copy_n(_RandomAccessIterator __first, _Size __n,
			   _ForwardIterator __result,
			   random_access_iterator_tag)
    { return std::uninitialized_copy(__first, __first + __n, __result); }

  







 
  template<typename _InputIterator, typename _Size, typename _ForwardIterator>
    inline _ForwardIterator
    uninitialized_copy_n(_InputIterator __first, _Size __n,
			 _ForwardIterator __result)
    { return std::__uninitialized_copy_n(__first, __n, __result,
					 std::__iterator_category(__first)); }


} 

















































 




 





























 




#pragma GCC visibility push(default)


namespace std
{
  
  template<class _E>
    class initializer_list
    {
    public:
      typedef _E 		value_type;
      typedef const _E& 	reference;
      typedef const _E& 	const_reference;
      typedef size_t 		size_type;
      typedef const _E* 	iterator;
      typedef const _E* 	const_iterator;

    private:
      iterator			_M_array;
      size_type			_M_len;

      
      constexpr initializer_list(const_iterator __a, size_type __l)
      : _M_array(__a), _M_len(__l) { }

    public:
      constexpr initializer_list() noexcept
      : _M_array(0), _M_len(0) { }

      
      constexpr size_type
      size() const noexcept { return _M_len; }

      
      constexpr const_iterator
      begin() const noexcept { return _M_array; }

      
      constexpr const_iterator
      end() const noexcept { return begin() + size(); }
    };

  



 
  template<class _Tp>
    constexpr const _Tp*
    begin(initializer_list<_Tp> __ils) noexcept
    { return __ils.begin(); }

  



 
  template<class _Tp>
    constexpr const _Tp*
    end(initializer_list<_Tp> __ils) noexcept
    { return __ils.end(); }
}

#pragma GCC visibility pop



namespace std 
{


  
  template<typename _Tp, typename _Alloc>
    struct _Vector_base
    {
      typedef typename __gnu_cxx::__alloc_traits<_Alloc>::template
        rebind<_Tp>::other _Tp_alloc_type;
      typedef typename __gnu_cxx::__alloc_traits<_Tp_alloc_type>::pointer
       	pointer;

      struct _Vector_impl 
      : public _Tp_alloc_type
      {
	pointer _M_start;
	pointer _M_finish;
	pointer _M_end_of_storage;

	_Vector_impl()
	: _Tp_alloc_type(), _M_start(), _M_finish(), _M_end_of_storage()
	{ }

	_Vector_impl(_Tp_alloc_type const& __a) noexcept
	: _Tp_alloc_type(__a), _M_start(), _M_finish(), _M_end_of_storage()
	{ }

	_Vector_impl(_Tp_alloc_type&& __a) noexcept
	: _Tp_alloc_type(std::move(__a)),
	  _M_start(), _M_finish(), _M_end_of_storage()
	{ }

	void _M_swap_data(_Vector_impl& __x) noexcept
	{
	  std::swap(_M_start, __x._M_start);
	  std::swap(_M_finish, __x._M_finish);
	  std::swap(_M_end_of_storage, __x._M_end_of_storage);
	}
      };
      
    public:
      typedef _Alloc allocator_type;

      _Tp_alloc_type&
      _M_get_Tp_allocator() noexcept
      { return *static_cast<_Tp_alloc_type*>(&this->_M_impl); }

      const _Tp_alloc_type&
      _M_get_Tp_allocator() const noexcept
      { return *static_cast<const _Tp_alloc_type*>(&this->_M_impl); }

      allocator_type
      get_allocator() const noexcept
      { return allocator_type(_M_get_Tp_allocator()); }

      _Vector_base()
      : _M_impl() { }

      _Vector_base(const allocator_type& __a) noexcept
      : _M_impl(__a) { }

      _Vector_base(size_t __n)
      : _M_impl()
      { _M_create_storage(__n); }

      _Vector_base(size_t __n, const allocator_type& __a)
      : _M_impl(__a)
      { _M_create_storage(__n); }

      _Vector_base(_Tp_alloc_type&& __a) noexcept
      : _M_impl(std::move(__a)) { }

      _Vector_base(_Vector_base&& __x) noexcept
      : _M_impl(std::move(__x._M_get_Tp_allocator()))
      { this->_M_impl._M_swap_data(__x._M_impl); }

      _Vector_base(_Vector_base&& __x, const allocator_type& __a)
      : _M_impl(__a)
      {
	if (__x.get_allocator() == __a)
	  this->_M_impl._M_swap_data(__x._M_impl);
	else
	  {
	    size_t __n = __x._M_impl._M_finish - __x._M_impl._M_start;
	    _M_create_storage(__n);
	  }
      }

      ~_Vector_base() noexcept
      { _M_deallocate(this->_M_impl._M_start, this->_M_impl._M_end_of_storage
		      - this->_M_impl._M_start); }

    public:
      _Vector_impl _M_impl;

      pointer
      _M_allocate(size_t __n)
      {
	typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
	return __n != 0 ? _Tr::allocate(_M_impl, __n) : pointer();
      }

      void
      _M_deallocate(pointer __p, size_t __n)
      {
	typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type> _Tr;
	if (__p)
	  _Tr::deallocate(_M_impl, __p, __n);
      }

    private:
      void
      _M_create_storage(size_t __n)
      {
	this->_M_impl._M_start = this->_M_allocate(__n);
	this->_M_impl._M_finish = this->_M_impl._M_start;
	this->_M_impl._M_end_of_storage = this->_M_impl._M_start + __n;
      }
    };


  



















 
  template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
    class vector : protected _Vector_base<_Tp, _Alloc>
    {
      
      typedef typename _Alloc::value_type                _Alloc_value_type;
      
      
      typedef _Vector_base<_Tp, _Alloc>			 _Base;
      typedef typename _Base::_Tp_alloc_type		 _Tp_alloc_type;
      typedef __gnu_cxx::__alloc_traits<_Tp_alloc_type>  _Alloc_traits;

    public:
      typedef _Tp					 value_type;
      typedef typename _Base::pointer                    pointer;
      typedef typename _Alloc_traits::const_pointer      const_pointer;
      typedef typename _Alloc_traits::reference          reference;
      typedef typename _Alloc_traits::const_reference    const_reference;
      typedef __gnu_cxx::__normal_iterator<pointer, vector> iterator;
      typedef __gnu_cxx::__normal_iterator<const_pointer, vector>
      const_iterator;
      typedef std::reverse_iterator<const_iterator>  const_reverse_iterator;
      typedef std::reverse_iterator<iterator>		 reverse_iterator;
      typedef size_t					 size_type;
      typedef ptrdiff_t					 difference_type;
      typedef _Alloc                        		 allocator_type;

    protected:
      using _Base::_M_allocate;
      using _Base::_M_deallocate;
      using _Base::_M_impl;
      using _Base::_M_get_Tp_allocator;

    public:
      
      

      

 
      vector()
      noexcept(is_nothrow_default_constructible<_Alloc>::value)
      : _Base() { }

      


 
      explicit
      vector(const allocator_type& __a) noexcept
      : _Base(__a) { }

      






 
      explicit
      vector(size_type __n, const allocator_type& __a = allocator_type())
      : _Base(__n, __a)
      { /* _M_default_initialize(__n); */ }

      






 
      vector(size_type __n, const value_type& __value,
	     const allocator_type& __a = allocator_type())
      : _Base(__n, __a)
      { _M_fill_initialize(__n, __value); }

      







 
      vector(const vector& __x)
      : _Base(__x.size(),
        _Alloc_traits::_S_select_on_copy(__x._M_get_Tp_allocator()))
      { this->_M_impl._M_finish =
	  std::__uninitialized_copy_a(__x.begin(), __x.end(),
				      this->_M_impl._M_start,
				      _M_get_Tp_allocator());
      }

      





 
      vector(vector&& __x) noexcept
      : _Base(std::move(__x)) { }

      
      vector(const vector& __x, const allocator_type& __a)
      : _Base(__x.size(), __a)
      { this->_M_impl._M_finish =
	  std::__uninitialized_copy_a(__x.begin(), __x.end(),
				      this->_M_impl._M_start,
				      _M_get_Tp_allocator());
      }

      
      vector(vector&& __rv, const allocator_type& __m)
      noexcept(_Alloc_traits::_S_always_equal())
      : _Base(std::move(__rv), __m)
      {
	if (__rv.get_allocator() != __m)
	  {
	    this->_M_impl._M_finish =
	      std::__uninitialized_move_a(__rv.begin(), __rv.end(),
					  this->_M_impl._M_start,
					  _M_get_Tp_allocator());
	    __rv.clear();
	  }
      }

      









 
      vector(initializer_list<value_type> __l,
	     const allocator_type& __a = allocator_type())
      : _Base(__a)
      {
	_M_range_initialize(__l.begin(), __l.end(),
			    random_access_iterator_tag());
      }

      














 
      template<typename _InputIterator,
	       typename = std::_RequireInputIter<_InputIterator>>
        vector(_InputIterator __first, _InputIterator __last,
	       const allocator_type& __a = allocator_type())
	: _Base(__a)
        { _M_initialize_dispatch(__first, __last, __false_type()); }

      




 
      ~vector() noexcept
      { std::_Destroy(this->_M_impl._M_start, this->_M_impl._M_finish,
		      _M_get_Tp_allocator()); }

      






 
      vector&
      operator=(const vector& __x);

      






 
      vector&
      operator=(vector&& __x) noexcept(_Alloc_traits::_S_nothrow_move())
      {
        constexpr bool __move_storage =
          _Alloc_traits::_S_propagate_on_move_assign()
          || _Alloc_traits::_S_always_equal();
#if 1
        _M_move_assign(std::move(__x), __bool_constant<__move_storage>());
#endif
	return *this;
      }


    };


} 






