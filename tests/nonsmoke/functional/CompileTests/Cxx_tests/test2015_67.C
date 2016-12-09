template<typename _Tp, _Tp __v>
struct integral_constant
   {
     static constexpr _Tp                  value = __v;
     typedef _Tp                           value_type;
     typedef integral_constant<_Tp, __v>   type;
     constexpr operator value_type() { return value; }
   };
  
template<typename _Tp, _Tp __v>
constexpr _Tp integral_constant<_Tp, __v>::value;

  /// The type used as a compile-time boolean with true value.
typedef integral_constant<bool, true>     true_type;

  /// The type used as a compile-time boolean with false value.
typedef integral_constant<bool, false>    false_type;


template<bool, typename, typename>
struct conditional;

// Primary template.
/// Define a member typedef @c type to one of two argument types.
template<bool _Cond, typename _Iftrue, typename _Iffalse>
struct conditional
   { typedef _Iftrue type; };

// Partial specialization for false.
template<typename _Iftrue, typename _Iffalse>
struct conditional<false, _Iftrue, _Iffalse>
   { typedef _Iffalse type; };

template<typename...>
struct __or_;

template<>
struct __or_<> // : public false_type
   { };

template<typename _B1>
struct __or_<_B1> : public _B1
   { };

template<typename>
struct is_function;

template<typename>
struct is_reference;

template<typename>
struct is_void;

template<typename _Pp>
struct __not_
   : public integral_constant<bool, !_Pp::value>
   { };


template<typename _B1, typename _B2>
struct __or_<_B1, _B2> : public conditional<_B1::value, _B1, _B2>::type
   { };

template<typename _B1, typename _B2, typename _B3, typename... _Bn>
struct __or_<_B1, _B2, _B3, _Bn...>
   : public conditional<_B1::value, _B1, __or_<_B2, _B3, _Bn...>>::type
   { };


template<typename _Tp>
struct is_object 
   : public __not_<__or_<is_function<_Tp>, is_reference<_Tp>,
                          is_void<_Tp>>>::type
   { };



is_object<int> XYZ;
