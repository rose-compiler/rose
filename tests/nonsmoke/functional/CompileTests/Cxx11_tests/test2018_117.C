// This is reproducer ROSE-35

namespace std
   {
     typedef __SIZE_TYPE__    size_t;
     typedef __PTRDIFF_TYPE__ ptrdiff_t;

     template<typename _Unqualified>
     class __match_cv_qualifiers
        {
          public:
               typedef  _Unqualified __type; // Part of type cycle.
        };

     template<typename _Tp>
     struct __make_unsigned    { };

     template<>
     struct __make_unsigned<long>
        {
          typedef unsigned long __type;
        };

     template<typename _Tp>
     class __make_unsigned_selector
        {
       // typedef __make_unsigned<_Tp> __unsignedt;
       // typedef typename __unsignedt::__type __unsigned_type;
#if 1
          typedef typename __make_unsigned<_Tp>::__type __unsigned_type;
#else
          typedef unsigned long __unsigned_type;
#endif
          typedef __match_cv_qualifiers<__unsigned_type> __cv_unsigned;

          public:
               typedef typename __cv_unsigned::__type __type; // Part of type cycle.
        };

     template<typename _Tp>
     struct make_unsigned
        { 
          typedef typename __make_unsigned_selector<_Tp>::__type type; // Part of type cycle.
        };
   } // namespace std


// static typename make_unsigned<unsigned long>::type foobar();
// std::make_unsigned<long>::type foobar();

#if 1
namespace __gnu_cxx
   {
     template<typename _Tp>
     class new_allocator
        {
          public:
          typedef _Tp        value_type;
 
          template<typename _Tp1>
          struct rebind
             {
               typedef new_allocator<_Tp1> other;
             };

          template<typename _Tp1>
          new_allocator(const new_allocator<_Tp1>&) { }
        };
   } // namespace __gnu_cxx


namespace std{
  template<typename _Tp>
    using __allocator_base = __gnu_cxx::new_allocator<_Tp>; 

  template<typename _Tp>
    class allocator: public __allocator_base<_Tp>    {    }; 

  template<typename _Alloc, typename _Tp>
    struct __alloctr_rebind    {
      typedef typename _Alloc::template rebind<_Tp>::other __type;
    };


  template<typename _Alloc>
    struct allocator_traits    {
      typedef typename _Alloc::value_type value_type;

    private:
      static value_type* _S_pointer_helper(...);
      typedef decltype(_S_pointer_helper((_Alloc*)0)) __pointer;

    public:
      typedef __pointer pointer;

    private:
      static typename std::ptrdiff_t _S_difference_type_helper(...);
      typedef decltype(_S_difference_type_helper((_Alloc*)0)) __difference_type;

      // These lines instead of the ones below cause:
      // identityTranslator: Cxx_Grammar.C:46765: SgType* SgType::stripType(unsigned char) const: Assertion `counter < 100' failed.
      //
      // static typename make_unsigned<__difference_type>::type _S_size_type_helper;
      // typedef decltype(_S_size_type_helper) __size_type;

#if 1
   // static typename make_unsigned<__difference_type>::type _S_size_type_helper(...);
   // typedef decltype(_S_size_type_helper((_Alloc*)0)) __size_type;
      typedef typename make_unsigned<__difference_type>::type __size_type;

    public:

    //NEEDED:
      typedef __size_type size_type;
#endif
    };

} // namespace std


namespace __gnu_cxx{

template<typename _Alloc>
  struct __alloc_traits
  : std::allocator_traits<_Alloc>  {

  public:

    static constexpr bool _S_nothrow_move()
    {return true;}

  };

}

 

namespace namespace_2 {
  typedef int typedef_3;
}

struct _Hash_node{};

template<typename _Alloc>
class _Hashtable{
  using __node_alloc_type =
    typename std::__alloctr_rebind<_Alloc, _Hash_node>::__type;

public:

  // NEEDED:
  typedef typename std::allocator_traits<
    typename std::__alloctr_rebind<
      __node_alloc_type,
      typename namespace_2::typedef_3>::__type>::pointer  pointer;

public:
  using const_iterator = int;

  // NEEDED:
  _Hashtable&
  operator=(_Hashtable&& __ht)
  noexcept
  // NEEDED:
  (__gnu_cxx::__alloc_traits<__node_alloc_type>::_S_nothrow_move())
 {}

};

template<class _Key>
class unordered_map{

  // NEEDED:
  _Hashtable<std::allocator<_Key>>  _M_h;

public:
  typedef typename _Hashtable<std::allocator<_Key>>::const_iterator  const_iterator;

  // NEEDED:
  unordered_map&
  operator=(unordered_map&&) = default;
};

// NEEDED:
namespace namespace_1 {
  typedef unordered_map<int>::const_iterator typedef_1;
}

#endif
