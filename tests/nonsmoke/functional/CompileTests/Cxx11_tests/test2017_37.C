


#include <bits/c++config.h>

// #include <bits/stl_relops.h>
// #include <bits/stl_pair.h>

#include <type_traits> // for std::__decay_and_strip too

namespace std _GLIBCXX_VISIBILITY(default)
{
  template<class _T1, class _T2>
    struct pair
    {
      typedef _T1 first_type;    /// @c first_type is the first bound type
      typedef _T2 second_type;   /// @c second_type is the second bound type
    };
  }

namespace std {
template < class T>
struct forward {
  //  typedef int type;
};
}

#if __cplusplus >= 201103L

// #include <bits/move.h>
// #include <initializer_list>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  template<class _Tp>
    class tuple_size;

  template<std::size_t _Int, class _Tp>
    class tuple_element;

   // Various functions which give std::pair a tuple-like interface.

  /// Partial specialization for std::pair
  template<class _Tp1, class _Tp2>
    struct tuple_size<std::pair<_Tp1, _Tp2>>
    : public integral_constant<std::size_t, 2> { };

  /// Partial specialization for std::pair
  template<class _Tp1, class _Tp2>
    struct tuple_element<0, std::pair<_Tp1, _Tp2>>
    { typedef _Tp1 type; };
 
  /// Partial specialization for std::pair
  template<class _Tp1, class _Tp2>
    struct tuple_element<1, std::pair<_Tp1, _Tp2>>
    { typedef _Tp2 type; };

  template<std::size_t _Int>
    struct __pair_get;

  template<>
    struct __pair_get<0>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp1&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp1>(__pair.first); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp1&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.first; }
    };

  template<>
    struct __pair_get<1>
    {
      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&
        __get(std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }

      template<typename _Tp1, typename _Tp2>
        static constexpr _Tp2&&
        __move_get(std::pair<_Tp1, _Tp2>&& __pair) noexcept
        { return std::forward<_Tp2>(__pair.second); }

      template<typename _Tp1, typename _Tp2>
        static constexpr const _Tp2&
        __const_get(const std::pair<_Tp1, _Tp2>& __pair) noexcept
        { return __pair.second; }
    };

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__get(__in); }

#if 0
  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&&
    get(std::pair<_Tp1, _Tp2>&& __in) noexcept
    { return __pair_get<_Int>::__move_get(std::move(__in)); }
#endif

  template<std::size_t _Int, class _Tp1, class _Tp2>
    constexpr const typename tuple_element<_Int, std::pair<_Tp1, _Tp2>>::type&
    get(const std::pair<_Tp1, _Tp2>& __in) noexcept
    { return __pair_get<_Int>::__const_get(__in); }

#if __cplusplus > 201103L

#define __cpp_lib_tuples_by_type 201304

  template <typename _Tp, typename _Up>
    constexpr _Tp&
    get(pair<_Tp, _Up>& __p) noexcept
    { return __p.first; }

  template <typename _Tp, typename _Up>
    constexpr const _Tp&
    get(const pair<_Tp, _Up>& __p) noexcept
    { return __p.first; }

  template <typename _Tp, typename _Up>
    constexpr _Tp&&
    get(pair<_Tp, _Up>&& __p) noexcept
    { return std::move(__p.first); }

  template <typename _Tp, typename _Up>
    constexpr _Tp&
    get(pair<_Up, _Tp>& __p) noexcept
    { return __p.second; }

  template <typename _Tp, typename _Up>
    constexpr const _Tp&
    get(const pair<_Up, _Tp>& __p) noexcept
    { return __p.second; }

  template <typename _Tp, typename _Up>
    constexpr _Tp&&
    get(pair<_Up, _Tp>&& __p) noexcept
    { return std::move(__p.second); }

#define __cpp_lib_exchange_function 201304

  /// Assign @p __new_val to @p __obj and return its previous value.
  template <typename _Tp, typename _Up = _Tp>
    inline _Tp
    exchange(_Tp& __obj, _Up&& __new_val)
    { return std::__exchange(__obj, std::forward<_Up>(__new_val)); }
#endif

  // Stores a tuple of indices.  Used by tuple and pair, and by bind() to
  // extract the elements in a tuple.
  template<size_t... _Indexes>
    struct _Index_tuple
    {
      typedef _Index_tuple<_Indexes..., sizeof...(_Indexes)> __next;
    };

  // Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
  template<size_t _Num>
    struct _Build_index_tuple
    {
      typedef typename _Build_index_tuple<_Num - 1>::__type::__next __type;
    };

  template<>
    struct _Build_index_tuple<0>
    {
      typedef _Index_tuple<> __type;
    };

#if __cplusplus > 201103L

#define __cpp_lib_integer_sequence 201304

  /// Class template integer_sequence
  template<typename _Tp, _Tp... _Idx>
    struct integer_sequence
    {
      typedef _Tp value_type;
      static constexpr size_t size() { return sizeof...(_Idx); }
    };

  template<typename _Tp, _Tp _Num,
	   typename _ISeq = typename _Build_index_tuple<_Num>::__type>
    struct _Make_integer_sequence;

  template<typename _Tp, _Tp _Num,  size_t... _Idx>
    struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
    {
      static_assert( _Num >= 0,
		     "Cannot make integer sequence of negative length" );

      typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
    };

  /// Alias template make_integer_sequence
  template<typename _Tp, _Tp _Num>
    using make_integer_sequence
      = typename _Make_integer_sequence<_Tp, _Num>::__type;

  /// Alias template index_sequence
  template<size_t... _Idx>
    using index_sequence = integer_sequence<size_t, _Idx...>;

  /// Alias template make_index_sequence
  template<size_t _Num>
    using make_index_sequence = make_integer_sequence<size_t, _Num>;

  /// Alias template index_sequence_for
  template<typename... _Types>
    using index_sequence_for = make_index_sequence<sizeof...(_Types)>;
#endif

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace

#endif



















#if 1
// #include <utility>
// #include <cmath>

#else

namespace std {
template < class T>
struct decay {
  typedef int type;
};
}
#endif

typedef int IndexType;

#if 1
namespace policy {
  struct invalid {};
  struct serial {};
  struct parstream {};
}
#endif


#if 1
template < typename exec_policy >
struct policy_traits_base;
#endif

#if 1
template <>
struct policy_traits_base< policy::serial > {
  static const bool impl_serial = true;
};
#endif

#if 1
template < typename exec_policy >
struct policy_traits_impl;
#endif

#if 1
template <>
struct policy_traits_impl< policy::parstream > :
policy_traits_base< policy::serial > {
  static const bool pol_serial = false;
};
#endif

#if 1
template < typename my_policy = policy::invalid >
using policy_traits = policy_traits_impl<my_policy>;
#endif

#if 1
template < typename my_policy, typename my_kernel >
struct ares_raja_api_kernel_type_check {
  // using policy_type = typename std::decay<my_policy>::type;
     using kernel_type = typename std::decay<my_kernel>::type;

  // static constexpr bool is_parallel_kernel = true;
  // static constexpr bool is_serial_kernel = true;

  // static constexpr bool is_parallel_policy = !policy_traits<policy_type>::impl_serial;
  // static constexpr bool is_serial_policy = policy_traits<policy_type>::impl_serial;
  // static constexpr bool value = ( is_serial_policy && is_serial_kernel ) || ( is_parallel_policy && is_parallel_kernel );
  static constexpr bool value = true;

};
#endif

#if 1
#if 1
    template < typename my_policy = policy::invalid,
             typename kernel_type >
               inline void
               for_all_2d( // xargs::index_only,
                   const IndexType& begin_i,
                   const IndexType& end_i,
                   const IndexType& begin_j,
                   const IndexType& end_j,
                   kernel_type&& kernel )
               {
#if 1
                   static_assert( ares_raja_api_kernel_type_check<my_policy, kernel_type>::value,"for_all: policy, lambda mismatch!" );
#endif
               }
#endif
#endif

void calcndx2d() // NodalAdv_t *nl, Domain_t *domain)
   {
     int jp = 0;
    for_all_2d< policy::parstream > (
        3, 10,
        2, 10,
        [=] (int i, int j) {

        int off = jp;
        });
}
