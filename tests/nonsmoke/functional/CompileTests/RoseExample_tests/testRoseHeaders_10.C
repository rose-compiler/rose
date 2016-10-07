typedef signed char int8_t;
typedef short int int16_t;
typedef int int32_t;
typedef long int int64_t;
typedef unsigned char uint8_t;
typedef short unsigned int uint16_t;
typedef unsigned int uint32_t;
typedef long unsigned int uint64_t;

typedef signed char int_least8_t;
typedef short int int_least16_t;
typedef int int_least32_t;
typedef long int int_least64_t;
typedef unsigned char uint_least8_t;
typedef short unsigned int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef long unsigned int uint_least64_t;

typedef signed char int_fast8_t;
typedef long int int_fast16_t;
typedef long int int_fast32_t;
typedef long int int_fast64_t;
typedef unsigned char uint_fast8_t;
typedef long unsigned int uint_fast16_t;
typedef long unsigned int uint_fast32_t;
typedef long unsigned int uint_fast64_t;

typedef long int intptr_t;
typedef long unsigned int uintptr_t;

typedef long int intmax_t;
typedef long unsigned int uintmax_t;


namespace std
{
  typedef long unsigned int 	size_t;
  typedef long int	ptrdiff_t;

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
  template<class _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> >
    class basic_string;

  template<> struct char_traits<char>;
 
  typedef basic_string<char>    string;
} 

namespace std 
{
  template<class _T1, class _T2>
    struct pair
    {
    };

  template<class _T1, class _T2>
    inline pair<_T1, _T2>
    make_pair(_T1 __x, _T2 __y)
    { return pair<_T1, _T2>(__x, __y); }
} 



namespace std 
{
  struct input_iterator_tag { };
  struct forward_iterator_tag : public input_iterator_tag { };
  struct bidirectional_iterator_tag : public forward_iterator_tag { };
  struct random_access_iterator_tag : public bidirectional_iterator_tag { };

  template<typename _Iterator>
    struct iterator_traits
    {
      typedef typename _Iterator::iterator_category iterator_category;
      typedef typename _Iterator::value_type        value_type;
      typedef typename _Iterator::difference_type   difference_type;
      typedef typename _Iterator::pointer           pointer;
      typedef typename _Iterator::reference         reference;
    };
  
  template<typename _Tp>
    struct iterator_traits<_Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef _Tp*                        pointer;
      typedef _Tp&                        reference;
    };

  template<typename _Iter>
    inline typename iterator_traits<_Iter>::iterator_category
    __iterator_category(const _Iter&)
    { return typename iterator_traits<_Iter>::iterator_category(); }
} 


namespace std 
{
  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    __distance(_InputIterator __first, _InputIterator __last,
               input_iterator_tag)
    {
      typename iterator_traits<_InputIterator>::difference_type __n = 0;
      return __n;
    }

  template<typename _InputIterator>
    inline typename iterator_traits<_InputIterator>::difference_type
    distance(_InputIterator __first, _InputIterator __last)
    {      
      return std::__distance(__first, __last,std::__iterator_category(__first));
    }
} 


namespace __gnu_cxx 
{
  using std::iterator_traits;
  template<typename _Iterator, typename _Container>
    class __normal_iterator
    {
    protected:
      typedef iterator_traits<_Iterator>		__traits_type;

    public:
      typedef typename __traits_type::iterator_category iterator_category;
      typedef typename __traits_type::value_type  	value_type;
      typedef typename __traits_type::difference_type 	difference_type;
      typedef typename __traits_type::reference 	reference;
      typedef typename __traits_type::pointer   	pointer;
    };
} 


namespace __gnu_cxx 
{
  template<typename _Tp>
    class new_allocator
    {
    public:
      typedef _Tp*       pointer;
      typedef const _Tp* const_pointer;

      template<typename _Tp1>
        struct rebind
        { typedef new_allocator<_Tp1> other; };
    };



}


namespace std 
   {
     template<typename _Tp>  class allocator : public __gnu_cxx::new_allocator<_Tp> {};
   }


namespace std 
{

  template<typename _CharT, typename _Traits, typename _Alloc>
    class basic_string
    {
      typedef typename _Alloc::template rebind<_CharT>::other _CharT_alloc_type;
    
    public:
      typedef typename _CharT_alloc_type::pointer	    pointer;
      typedef typename _CharT_alloc_type::const_pointer	    const_pointer;
      typedef __gnu_cxx::__normal_iterator<const_pointer, basic_string>
                                                            const_iterator;
    public:
      
      basic_string() {}

      basic_string(const _CharT* __s, const _Alloc& __a = _Alloc());
  };


} 




namespace boost{
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
}

namespace boost{
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
}


namespace boost
{

  using ::int8_t;
  using ::int_least8_t;
  using ::int_fast8_t;
  using ::uint8_t;
  using ::uint_least8_t;
  using ::uint_fast8_t;

  using ::int16_t;
  using ::int_least16_t;
  using ::int_fast16_t;
  using ::uint16_t;
  using ::uint_least16_t;
  using ::uint_fast16_t;

  using ::int32_t;
  using ::int_least32_t;
  using ::int_fast32_t;
  using ::uint32_t;
  using ::uint_least32_t;
  using ::uint_fast32_t;


  using ::int64_t;
  using ::int_least64_t;
  using ::int_fast64_t;
  using ::uint64_t;
  using ::uint_least64_t;
  using ::uint_fast64_t;


  using ::intmax_t;
  using ::uintmax_t;

} 










namespace boost {
namespace noncopyable_  
{
  class noncopyable
  {
  protected:
      noncopyable() {}
      ~noncopyable() {}
  private:  
      noncopyable( const noncopyable& );
      noncopyable& operator=( const noncopyable& );
  };
}

typedef noncopyable_::noncopyable noncopyable;
} 


namespace mpl_ { namespace aux {} }
namespace boost { namespace mpl { using namespace mpl_; 
namespace aux { using namespace mpl_::aux; }
}}



namespace mpl_ {
struct integral_c_tag { static const int value = 0; };
}
namespace boost { namespace mpl { using ::mpl_::integral_c_tag; } }



namespace mpl_ {

template< bool C_ > struct bool_;


typedef bool_<true> true_;
typedef bool_<false> false_;

}

namespace boost { namespace mpl { using ::mpl_::bool_; } }
namespace boost { namespace mpl { using ::mpl_::true_; } }
namespace boost { namespace mpl { using ::mpl_::false_; } }


namespace mpl_ {

template< bool C_ > struct bool_
{
    static const bool value = C_;
    typedef integral_c_tag tag;
    typedef bool_ type;
    typedef bool value_type;
     operator bool() const { return this->value; }
};

template< bool C_ >
bool const bool_<C_>::value;

}






























namespace mpl_ {

template< typename T, T N > struct integral_c;

}
namespace boost { namespace mpl { using ::mpl_::integral_c; } }

namespace mpl_ {

template< typename T, T N >
struct integral_c
{
    static const T value = N;


    typedef integral_c type;
    typedef T value_type;
    typedef integral_c_tag tag;
    typedef integral_c< T, static_cast<T>((value + 1)) > next;
    typedef integral_c< T, static_cast<T>((value - 1)) > prior;
    operator T() const { return static_cast<T>(this->value); } 
};

template< typename T, T N >
T const integral_c< T, N > ::value;
}

namespace mpl_ {

template< bool C >
struct integral_c<bool, C>
{
    static const bool value = C;
    typedef integral_c_tag tag;
    typedef integral_c type;
    typedef bool value_type;
    operator bool() const { return this->value; }
};
}


namespace boost{

template <class T, T val>
struct integral_constant : public mpl::integral_c<T, val>
{
   typedef integral_constant<T,val> type;
};

template<> struct integral_constant<bool,true> : public mpl::true_ 
{
   typedef integral_constant<bool,true> type;
};
template<> struct integral_constant<bool,false> : public mpl::false_ 
{
   typedef integral_constant<bool,false> type;
};

typedef integral_constant<bool,true> true_type;
typedef integral_constant<bool,false> false_type;

}



namespace boost {
template< typename T, typename U > struct is_same : public ::boost::integral_constant<bool,false> { public:   }; 
template< typename T > struct is_same< T,T > : public ::boost::integral_constant<bool,true> { public:   };
} 


namespace boost {
template< typename T > struct is_lvalue_reference : public ::boost::integral_constant<bool,false> { public:   }; 
template< typename T > struct is_lvalue_reference< T& > : public ::boost::integral_constant<bool,true> { public:  };
} 



namespace boost {

template< typename T > struct is_rvalue_reference : public ::boost::integral_constant<bool,false> { public:   }; 

} 


namespace boost {
namespace type_traits {

typedef char yes_type;
struct no_type
{
   char padding[8];
};

} 
} 



namespace boost {
namespace type_traits {

template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false>
struct ice_or;

template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_or
{
    static const bool value = true;
};

template <>
struct ice_or<false, false, false, false, false, false, false>
{
    static const bool value = false;
};

} 
} 


namespace boost {
namespace type_traits {

template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true>
struct ice_and;

template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_and
{
    static const bool value = false;
};

template <>
struct ice_and<true, true, true, true, true, true, true>
{
    static const bool value = true;
};

} 
} 



namespace boost {
namespace type_traits {

template <bool b>
struct ice_not
{
    static const bool value = true;
};

template <>
struct ice_not<true>
{
    static const bool value = false;
};

} 
} 



namespace boost {

namespace detail {

template <typename T>
struct is_reference_impl
{
   static const bool value = (::boost::type_traits::ice_or< ::boost::is_lvalue_reference<T> ::value, ::boost::is_rvalue_reference<T> ::value > ::value);
};

} 

template< typename T > struct is_reference : public ::boost::integral_constant<bool,::boost::detail::is_reference_impl<T> ::value> { public:   }; 

} 


namespace boost {
namespace detail {

template <typename T> struct cv_traits_imp {};

template <typename T>
struct cv_traits_imp<T*>
{
    static const bool is_const = false;
    static const bool is_volatile = false;
    typedef T unqualified_type;
};

template <typename T>
struct cv_traits_imp<const T * >
{
    static const bool is_const = true;
    static const bool is_volatile = false;
    typedef T unqualified_type;
};

template <typename T>
struct cv_traits_imp<volatile T * >
{
    static const bool is_const = false;
    static const bool is_volatile = true;
    typedef T unqualified_type;
};

template <typename T>
struct cv_traits_imp<const volatile T * >
{
    static const bool is_const = true;
    static const bool is_volatile = true;
    typedef T unqualified_type;
};

} 
} 


namespace boost {

namespace detail{
template <class T>
struct is_volatile_rval_filter
{
   static const bool value = ::boost::detail::cv_traits_imp<T * > ::is_volatile;
};
}

template< typename T > struct is_volatile : public ::boost::integral_constant<bool,::boost::detail::is_volatile_rval_filter<T> ::value> { public:   }; 
template< typename T > struct is_volatile< T& > : public ::boost::integral_constant<bool,false> { public:  };
} 

namespace boost {
template< typename T > struct is_array : public ::boost::integral_constant<bool,false> { public:   }; 
template< typename T, std::size_t N > struct is_array< T[N] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T, std::size_t N > struct is_array< T const[N] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T, std::size_t N > struct is_array< T volatile[N] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T, std::size_t N > struct is_array< T const volatile[N] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T > struct is_array< T[] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T > struct is_array< T const[] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T > struct is_array< T volatile[] > : public ::boost::integral_constant<bool,true> { public:  };
template< typename T > struct is_array< T const volatile[] > : public ::boost::integral_constant<bool,true> { public:  };
} 


namespace boost {

template< typename T > struct is_integral : public ::boost::integral_constant<bool,false> { public:   }; 

template< > struct is_integral< unsigned char > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned char const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned char volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned char const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< unsigned short > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned short const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned short volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned short const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< unsigned int > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned int const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned int volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned int const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< unsigned long > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned long const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned long volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< unsigned long const volatile > : public ::boost::integral_constant<bool,true> { public:   };

template< > struct is_integral< signed char > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed char const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed char volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed char const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< signed short > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed short const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed short volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed short const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< signed int > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed int const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed int volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed int const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< signed long > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed long const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed long volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< signed long const volatile > : public ::boost::integral_constant<bool,true> { public:   };

template< > struct is_integral< bool > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< bool const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< bool volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< bool const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< char > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< char const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< char volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< char const volatile > : public ::boost::integral_constant<bool,true> { public:   };




template< > struct is_integral< wchar_t > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< wchar_t const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< wchar_t volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< wchar_t const volatile > : public ::boost::integral_constant<bool,true> { public:   };




template< > struct is_integral< ::boost::ulong_long_type > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::ulong_long_type const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::ulong_long_type volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::ulong_long_type const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< ::boost::long_long_type > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::long_long_type const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::long_long_type volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< ::boost::long_long_type const volatile > : public ::boost::integral_constant<bool,true> { public:   };
        
template< > struct is_integral< boost::int128_type > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::int128_type const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::int128_type volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::int128_type const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_integral< boost::uint128_type > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::uint128_type const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::uint128_type volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_integral< boost::uint128_type const volatile > : public ::boost::integral_constant<bool,true> { public:   };


} 



namespace boost {


template< typename T > struct is_float : public ::boost::integral_constant<bool,false> { public:   }; 
template< > struct is_float< float > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< float const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< float volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< float const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_float< double > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< double const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< double volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< double const volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_float< long double > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< long double const > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< long double volatile > : public ::boost::integral_constant<bool,true> { public:   }; template< > struct is_float< long double const volatile > : public ::boost::integral_constant<bool,true> { public:   };

} 



namespace boost {

namespace detail {

template< typename T >
struct is_arithmetic_impl
{ 
    static const bool value = (::boost::type_traits::ice_or< ::boost::is_integral<T> ::value, ::boost::is_float<T> ::value > ::value); 
};

} 


template< typename T > struct is_arithmetic : public ::boost::integral_constant<bool,::boost::detail::is_arithmetic_impl<T> ::value> { public:   }; 

} 


namespace boost {
template< typename T > struct is_void : public ::boost::integral_constant<bool,false> { public:   }; 
template< > struct is_void< void > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_void< void const > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_void< void volatile > : public ::boost::integral_constant<bool,true> { public:   };
template< > struct is_void< void const volatile > : public ::boost::integral_constant<bool,true> { public:   };
} 

namespace boost {
namespace detail{
template <class T>
struct is_abstract_imp
{
   static const bool value = __is_abstract(T);
};
}

template< typename T > struct is_abstract : public ::boost::integral_constant<bool,::boost::detail::is_abstract_imp<T> ::value> { public:   }; 
} 


namespace boost {
namespace detail {

template <typename T>
struct add_reference_rvalue_layer
{
    typedef T& type;
};

template <typename T>
struct add_reference_impl
{
    typedef typename add_reference_rvalue_layer<T>::type type;
};

template< typename T > struct add_reference_impl<T& > { public: typedef T& type; };

template< > struct add_reference_impl<void> { public: typedef void type; };
template< > struct add_reference_impl<void const> { public: typedef void const type; };
template< > struct add_reference_impl<void volatile> { public: typedef void volatile type; };
template< > struct add_reference_impl<void const volatile> { public: typedef void const volatile type; };
} 

template< typename T > struct add_reference { public: typedef typename boost::detail::add_reference_impl<T> ::type type;  }; 
} 



namespace boost{

template< typename T > struct add_lvalue_reference { public: typedef typename boost::add_reference<T> ::type type;  }; 


}



namespace boost {

namespace type_traits_detail {

    template <typename T, bool b>
    struct add_rvalue_reference_helper
    { typedef T   type; };


    template <typename T>
    struct add_rvalue_reference_imp
    {
       typedef typename boost::type_traits_detail::add_rvalue_reference_helper
                  <T, (is_void<T>::value == false && is_reference<T>::value == false) >::type type;
    };

}

template< typename T > struct add_rvalue_reference { public: typedef typename boost::type_traits_detail::add_rvalue_reference_imp<T> ::type type;  }; 

}  



namespace boost {
namespace type_traits {


struct false_result
{
    template <typename T> struct result_
    {
        static const bool value = false;
    };
};

}} 


namespace boost {
namespace type_traits {

template <class R>
struct is_function_ptr_helper
{
    static const bool value = false;
};



template <class R >
struct is_function_ptr_helper<R (*)()> { static const bool value = true; };
template <class R >
struct is_function_ptr_helper<R (*)( ...)> { static const bool value = true; };
template <class R , class T0>
struct is_function_ptr_helper<R (*)( T0)> { static const bool value = true; };
template <class R , class T0>
struct is_function_ptr_helper<R (*)( T0 ...)> { static const bool value = true; };
template <class R , class T0 , class T1>
struct is_function_ptr_helper<R (*)( T0 , T1)> { static const bool value = true; };
template <class R , class T0 , class T1>
struct is_function_ptr_helper<R (*)( T0 , T1 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 ...)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24)> { static const bool value = true; };
template <class R , class T0 , class T1 , class T2 , class T3 , class T4 , class T5 , class T6 , class T7 , class T8 , class T9 , class T10 , class T11 , class T12 , class T13 , class T14 , class T15 , class T16 , class T17 , class T18 , class T19 , class T20 , class T21 , class T22 , class T23 , class T24>
struct is_function_ptr_helper<R (*)( T0 , T1 , T2 , T3 , T4 , T5 , T6 , T7 , T8 , T9 , T10 , T11 , T12 , T13 , T14 , T15 , T16 , T17 , T18 , T19 , T20 , T21 , T22 , T23 , T24 ...)> { static const bool value = true; };

} 
} 



namespace boost {


namespace detail {

template<bool is_ref = true>
struct is_function_chooser
    : public ::boost::type_traits::false_result
{
};

template <>
struct is_function_chooser<false>
{
    template< typename T > struct result_
        : public ::boost::type_traits::is_function_ptr_helper<T*>
    {
    };
};

template <typename T>
struct is_function_impl
    : public is_function_chooser< ::boost::is_reference<T>::value >
        ::template result_<T>
{
};


} 


template< typename T > struct is_function : public ::boost::integral_constant<bool,::boost::detail::is_function_impl<T> ::value> { public:   }; 
} 



namespace boost {
namespace detail {

struct any_conversion
{
    template <typename T> any_conversion(const volatile T&);
    template <typename T> any_conversion(const T&);
    template <typename T> any_conversion(volatile T&);
    template <typename T> any_conversion(T&);
};

template <typename T> struct checker
{
    static boost::type_traits::no_type _m_check(any_conversion ...);
    static boost::type_traits::yes_type _m_check(T, int);
};

template <typename From, typename To>
struct is_convertible_basic_impl
{
    typedef typename add_lvalue_reference<From>::type lvalue_type;
    typedef typename add_rvalue_reference<From>::type rvalue_type;
    static lvalue_type _m_from;
    static bool const value =
        sizeof( boost::detail::checker<To>::_m_check(_m_from, 0) )
        == sizeof(::boost::type_traits::yes_type);
};


template <typename From, typename To>
struct is_convertible_impl
{
    static const bool value = (::boost::type_traits::ice_and< ::boost::type_traits::ice_or< ::boost::detail::is_convertible_basic_impl<From,To> ::value, ::boost::is_void<To> ::value > ::value, ::boost::type_traits::ice_not< ::boost::is_array<To> ::value > ::value, ::boost::type_traits::ice_not< ::boost::is_function<To> ::value > ::value > ::value);
};

template <bool trivial1, bool trivial2, bool abstract_target>
struct is_convertible_impl_select
{
   template <class From, class To>
   struct rebind
   {
      typedef is_convertible_impl<From, To> type;
   };
};

template <>
struct is_convertible_impl_select<true, true, false>
{
   template <class From, class To>
   struct rebind
   {
      typedef true_type type;
   };
};

template <>
struct is_convertible_impl_select<false, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <>
struct is_convertible_impl_select<true, false, true>
{
   template <class From, class To>
   struct rebind
   {
      typedef false_type type;
   };
};

template <typename From, typename To>
struct is_convertible_impl_dispatch_base
{
   typedef is_convertible_impl_select< 
      ::boost::is_arithmetic<From>::value, 
      ::boost::is_arithmetic<To>::value,
      
      ::boost::is_abstract<To>::value
   > selector;
   typedef typename selector::template rebind<From, To> isc_binder;
   typedef typename isc_binder::type type;
};

template <typename From, typename To>
struct is_convertible_impl_dispatch 
   : public is_convertible_impl_dispatch_base<From, To>::type
{};

    template< > struct is_convertible_impl< void,void > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void,void const > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void,void volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void,void const volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const,void > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const,void const > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const,void volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const,void const volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void volatile,void > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void volatile,void const > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void volatile,void volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void volatile,void const volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const volatile,void > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const volatile,void const > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const volatile,void volatile > { public: static const bool value = (true); }; template< > struct is_convertible_impl< void const volatile,void const volatile > { public: static const bool value = (true); };

template< typename To > struct is_convertible_impl< void,To > { public: static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void > { public: static const bool value = (false); };
template< typename To > struct is_convertible_impl< void const,To > { public: static const bool value = (false); };
template< typename To > struct is_convertible_impl< void volatile,To > { public: static const bool value = (false); };
template< typename To > struct is_convertible_impl< void const volatile,To > { public: static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void const > { public: static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void volatile > { public: static const bool value = (false); };
template< typename From > struct is_convertible_impl< From,void const volatile > { public: static const bool value = (false); };

} 

template< typename From, typename To > struct is_convertible : public ::boost::integral_constant<bool,(::boost::detail::is_convertible_impl_dispatch<From,To> ::value)> { public:   }; 


} 

namespace boost {
namespace detail{
template <class T>
struct remove_rvalue_ref
{
   typedef T type;
};
} 
template< typename T > struct remove_reference { public: typedef typename boost::detail::remove_rvalue_ref<T> ::type type;  }; 
template< typename T > struct remove_reference<T& > { public: typedef T type; };
} 


namespace boost {
namespace detail{

template <class T>
struct rvalue_ref_filter_rem_cv
{
   typedef typename boost::detail::cv_traits_imp<T * >::unqualified_type type;
};
}

template< typename T > struct remove_cv { public: typedef typename boost::detail::rvalue_ref_filter_rem_cv<T> ::type type;  }; 
template< typename T > struct remove_cv<T& > { public: typedef T& type; };
template< typename T, std::size_t N > struct remove_cv<T const[N]> { public: typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T volatile[N]> { public: typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T const volatile[N]> { public: typedef T type[N]; };
} 

namespace boost {
template< typename T > struct remove_bounds { public: typedef T type;  }; 
template< typename T, std::size_t N > struct remove_bounds<T[N]> { public: typedef T type; };
template< typename T, std::size_t N > struct remove_bounds<T const[N]> { public: typedef T const type; };
template< typename T, std::size_t N > struct remove_bounds<T volatile[N]> { public: typedef T volatile type; };
template< typename T, std::size_t N > struct remove_bounds<T const volatile[N]> { public: typedef T const volatile type; };
template< typename T > struct remove_bounds<T[]> { public: typedef T type; };
template< typename T > struct remove_bounds<T const[]> { public: typedef T const type; };
template< typename T > struct remove_bounds<T volatile[]> { public: typedef T volatile type; };
template< typename T > struct remove_bounds<T const volatile[]> { public: typedef T const volatile type; };
} 


namespace boost {
namespace detail {

template <typename T>
struct add_pointer_impl
{
    typedef typename remove_reference<T>::type no_ref_type;
    typedef no_ref_type* type;
};
} 

template< typename T > struct add_pointer { public: typedef typename boost::detail::add_pointer_impl<T> ::type type;  }; 
} 


#if 0

namespace boost { namespace mpl { namespace aux {

template< typename T > struct value_type_wknd
{
    typedef typename T::value_type type;
};


}}}

#endif


namespace mpl_ {

struct void_;

}
namespace boost { namespace mpl { using ::mpl_::void_; } }



namespace mpl_ {


struct na
{
    typedef na type;
    enum { value = 0 };
};

}


namespace boost { namespace mpl {

template< 
      typename T = na
    , typename Tag = void_
    
    >
struct lambda;

}}



namespace boost { namespace mpl {


template<
      bool C
    , typename T1
    , typename T2
    >
struct if_c
{
    typedef T1 type;
};

template<
      typename T1
    , typename T2
    >
struct if_c<false,T1,T2>
{
    typedef T2 type;
};



template<
      typename T1 = na
    , typename T2 = na
    , typename T3 = na
    >
struct if_
{
 private:
    
    typedef if_c<
          static_cast<bool>(T1::value)
        , T2
        , T3
        > almost_type_;
 
 public:
    typedef typename almost_type_::type type;
    
    
};


template< > struct if_<  na , na , na > { template<  typename T1 , typename T2 , typename T3 , typename T4 =na  , typename T5 =na  > struct apply : if_<  T1 , T2 , T3 > { }; }; template< typename Tag > struct lambda< if_<  na , na , na > , Tag  > { typedef false_ is_le; typedef if_<  na , na , na > result_; typedef if_<  na , na , na > type; };   

}}


namespace boost { namespace mpl {

template<
      typename C = na
    , typename F1 = na
    , typename F2 = na
    >
struct eval_if
    : if_<C,F1,F2>::type
{
    
};



template<
      bool C
    , typename F1
    , typename F2
    >
struct eval_if_c
    : if_c<C,F1,F2>::type
{
};

template< > struct eval_if<  na , na , na > { template<  typename T1 , typename T2 , typename T3 , typename T4 =na  , typename T5 =na  > struct apply : eval_if<  T1 , T2 , T3 > { }; }; template< typename Tag > struct lambda< eval_if<  na , na , na > , Tag  > { typedef false_ is_le; typedef eval_if<  na , na , na > result_; typedef eval_if<  na , na , na > type; };   

}}




#if 1


namespace boost { namespace mpl {

template<
      typename T = na
    >
struct identity
{
    typedef T type;
    
};

template<
      typename T = na
    >
struct make_identity
{
    typedef identity<T> type;
    
};

template< > struct identity<  na > { template<  typename T1 , typename T2 =na  , typename T3 =na  , typename T4 =na  , typename T5 =na  > struct apply : identity<  T1 > { }; }; template< typename Tag > struct lambda< identity<  na > , Tag  > { typedef false_ is_le; typedef identity<  na > result_; typedef identity<  na > type; };  
template< > struct make_identity<  na > { template<  typename T1 , typename T2 =na  , typename T3 =na  , typename T4 =na  , typename T5 =na  > struct apply : make_identity<  T1 > { }; }; template< typename Tag > struct lambda< make_identity<  na > , Tag  > { typedef false_ is_le; typedef make_identity<  na > result_; typedef make_identity<  na > type; };  

}}
#endif


#if 1

namespace boost 
{

    template< class T >
    struct decay
    {
    private:
        typedef typename remove_reference<T>::type Ty;
    public:
        typedef typename mpl::eval_if< 
            is_array<Ty>,
            mpl::identity<typename remove_bounds<Ty>::type*>,
            typename mpl::eval_if< 
                is_function<Ty>,
                add_pointer<Ty>,
                mpl::identity<Ty>
            >
        >::type type;
    };
    
} 

#endif



namespace boost {
namespace detail{

template <class T>
struct is_const_rvalue_filter
{
   static const bool value = ::boost::detail::cv_traits_imp<T * > ::is_const;
};
}

template< typename T > struct is_const : public ::boost::integral_constant<bool,::boost::detail::is_const_rvalue_filter<T> ::value> { public:   }; 
template< typename T > struct is_const< T& > : public ::boost::integral_constant<bool,false> { public:  };
} 


namespace boost {

namespace detail {

template <typename B, typename D>
struct is_base_and_derived_impl
{
    typedef typename remove_cv<B>::type ncvB;
    typedef typename remove_cv<D>::type ncvD;

    static const bool value = ((__is_base_of(B,D) && !is_same<B,D> ::value) && ! ::boost::is_same<ncvB,ncvD> ::value);
};
} 

template< typename Base, typename Derived > struct is_base_and_derived : public ::boost::integral_constant<bool,(::boost::detail::is_base_and_derived_impl<Base,Derived> ::value)> { public:   }; 

template< typename Base, typename Derived > struct is_base_and_derived< Base&,Derived > : public ::boost::integral_constant<bool,false> { public:  };
template< typename Base, typename Derived > struct is_base_and_derived< Base,Derived& > : public ::boost::integral_constant<bool,false> { public:  };
template< typename Base, typename Derived > struct is_base_and_derived< Base&,Derived& > : public ::boost::integral_constant<bool,false> { public:  };


} 



extern "C++" {

namespace __cxxabiv1
{
  //  class __class_type_info;
} 


namespace std
{ 
  class type_info
  {
  public:
    virtual ~type_info();
    const char* name() const 
    { return __name[0] == '*' ? __name + 1 : __name; }
    
    bool before(const type_info& __arg) const ;
    bool operator==(const type_info& __arg) const ;
    bool operator!=(const type_info& __arg) const 
    { return !operator==(__arg); }
    virtual bool __is_pointer_p() const;
    virtual bool __is_function_p() const;
    virtual bool __do_catch(const type_info *__thr_type, void **__thr_obj, unsigned __outer) const;
 // virtual bool __do_upcast(const __cxxabiv1::__class_type_info *__target, void **__obj_ptr) const;
  protected:
    const char *__name;

    explicit type_info(const char *__n): __name(__n) { }

  private:
    type_info& operator=(const type_info&);
    type_info(const type_info&);
  };

} 

} 



    
namespace boost { namespace mpl {

namespace aux {

template< bool C_, typename T1, typename T2, typename T3, typename T4 >
struct or_impl
    : true_
{
};

template< typename T1, typename T2, typename T3, typename T4 >
struct or_impl< false,T1,T2,T3,T4 >
    : or_impl<
          T1::type::value
        , T2, T3, T4
        , false_
        >
{
};

template<>
struct or_impl<
          false
        , false_, false_, false_, false_
        >
    : false_
{
};

} 

template<
      typename T1 = na
    , typename T2 = na
    , typename T3 = false_, typename T4 = false_, typename T5 = false_
    >
struct or_

    : aux::or_impl<
          T1::type::value
        , T2, T3, T4, T5
        >

{
};

template< > struct or_<  na , na > { template<  typename T1 , typename T2 , typename T3 =na  , typename T4 =na  , typename T5 =na  > struct apply : or_<  T1 , T2 > { }; };  template< typename Tag > struct lambda< or_<  na , na > , Tag  > { typedef false_ is_le; typedef or_<  na , na > result_; typedef or_<  na , na > type; };  

}}



namespace boost { namespace mpl {

namespace aux {

template< bool C_, typename T1, typename T2, typename T3, typename T4 > struct and_impl : false_ {};

template< typename T1, typename T2, typename T3, typename T4 > struct and_impl< true,T1,T2,T3,T4 > : and_impl< T1::type::value, T2, T3, T4, true_ > {};

template<> struct and_impl<true, true_, true_, true_, true_ > : true_ {};

} 

template< typename T1 = na, typename T2 = na, typename T3 = true_, typename T4 = true_, typename T5 = true_ > struct and_ : aux::and_impl<T1::type::value, T2, T3, T4, T5> {};

    // template< > struct and_<  na , na > { template<  typename T1 , typename T2 , typename T3 =na  , typename T4 =na  , typename T5 =na  > struct apply : and_<  T1 , T2 > { }; };  template< typename Tag > struct lambda< and_<  na , na > , Tag  > { typedef false_ is_le; typedef and_<  na , na > result_; typedef and_<  na , na > type; };  

}}



namespace boost { namespace mpl {

namespace aux {

   template< long C_ > struct not_impl : bool_<!C_> {};

} 

template< typename T = na > struct not_ : aux::not_impl<T::type::value> {};

// template< > struct not_<  na > { template<  typename T1 , typename T2 =na  , typename T3 =na  , typename T4 =na  , typename T5 =na  > struct apply : not_<  T1 > { }; }; template< typename Tag > struct lambda< not_<  na > , Tag  > { typedef false_ is_le; typedef not_<  na > result_; typedef not_<  na > type; };   

}}



namespace boost { namespace typeindex {


template <class Derived, class TypeInfo>
class type_index_facade {

public:
    typedef TypeInfo                                type_info_t;
    
    inline const char* name() const  {
        return 0L;
    }
    
    inline std::string pretty_name() const {
      return std::string();
    }
    
    inline bool equal(const Derived& rhs) const  {
        return false;
    }
    
    inline bool before(const Derived& rhs) const  {
        return false;
    }
    
    inline std::size_t hash_code() const  {
        return 0;
    }

};

template <class Derived, class TypeInfo>
inline bool operator == (const type_index_facade<Derived, TypeInfo>& lhs, const type_index_facade<Derived, TypeInfo>& rhs)  {
    return static_cast<Derived const&>(lhs).equal(static_cast<Derived const&>(rhs));
}

template <class Derived, class TypeInfo>
inline bool operator == (const TypeInfo& lhs, const type_index_facade<Derived, TypeInfo>& rhs)  {
    return Derived(lhs) == rhs;
}

}} 


#if 1


# pragma once

namespace boost { namespace typeindex {

class stl_type_index
    : public type_index_facade<
        stl_type_index, 
            std::type_info
    > 
{
public:
    typedef std::type_info type_info_t;

private:
    const type_info_t* data_;

public:
#if 0
    inline stl_type_index() 
        : data_(&typeid(void))
    {}
#endif

#if 1
    inline stl_type_index(const type_info_t& data) 
        : data_(&data)
    {}
#endif

    inline const type_info_t&  type_info() const ;

#if 0
    inline const char*  raw_name() const ;
    inline const char*  name() const ;
    inline std::string  pretty_name() const;

    inline std::size_t  hash_code() const ;
    inline bool         equal(const stl_type_index& rhs) const ;
    inline bool         before(const stl_type_index& rhs) const ;
#endif

    template <class T>
    inline static stl_type_index type_id() ;

#if 0
    template <class T>
    inline static stl_type_index type_id_with_cvr() ;

    template <class T>
    inline static stl_type_index type_id_runtime(const T& value) ;
#endif

};




template <class T>
inline stl_type_index stl_type_index::type_id()  {
    typedef typename boost::remove_reference<T>::type no_ref_t;
    typedef typename boost::remove_cv<no_ref_t>::type no_cvr_prefinal_t;

        typedef no_cvr_prefinal_t no_cvr_t;

    return typeid(no_cvr_t);
}

}}

#endif



namespace boost { namespace typeindex {

    typedef boost::typeindex::stl_type_index type_index;

    typedef type_index::type_info_t type_info;

template <class T>
inline type_index type_id()  {
    return type_index::type_id<T>();
}

}} 

namespace boost
{
    class any
    {
    public: 
        template<typename ValueType> any(const ValueType & value) : content(new holder<typename remove_cv<typename decay<const ValueType>::type>::type>(value)) {}

    public: 

      bool empty() const; // { return !content; }

      void clear(); // { any().swap(*this); }

      const boost::typeindex::type_info& type() const; // { return content ? content->type() : boost::typeindex::type_id<void>().type_info(); }

    private: 

        class placeholder
        {
        public: 

          // virtual ~placeholder() {}

        public: 

             virtual const boost::typeindex::type_info& type() const  = 0;

             virtual placeholder * clone() const = 0;

        };

        template<typename ValueType>
        class holder : public placeholder
        {
        public: 

          holder(const ValueType & value)
            //              : held(value)
              {}

        public: 

            virtual const boost::typeindex::type_info& type() const { return boost::typeindex::type_id<ValueType>().type_info(); }
#if 1
            virtual placeholder * clone() const; // { return new holder(held); }
#endif
        public: 

            ValueType held;

#if 0
        private: 
            holder & operator=(const holder &);
#endif

        };


    private: 
#if 1
        template<typename ValueType>
        friend ValueType * any_cast(any *);

        template<typename ValueType>
        friend ValueType * unsafe_any_cast(any *);
#endif
        placeholder * content;

    };

    template<typename ValueType>
    ValueType * any_cast(any * operand) 
    {
        return operand && operand->type() == boost::typeindex::type_id<ValueType>()
            ? &static_cast<any::holder<typename remove_cv<ValueType>::type> *>(operand->content)->held
            : 0;
    }

    template<typename ValueType>
    ValueType any_cast(any & operand)
    {
        typedef typename remove_reference<ValueType>::type nonref;

        nonref * result = any_cast<nonref>(&operand);
        typedef typename boost::mpl::if_<
            boost::is_reference<ValueType>,
            ValueType,
            typename boost::add_reference<ValueType>::type
        >::type ref_type;

        return static_cast<ref_type>(*result);
    }

    template<typename ValueType>
    inline ValueType any_cast(const any & operand)
    {
        typedef typename remove_reference<ValueType>::type nonref;
        return any_cast<const nonref &>(const_cast<any &>(operand));
    }

}

namespace boost
{

namespace detail
{

using std::iterator_traits;
using std::distance;

} 

} 

namespace boost {
namespace iterators {

     template <class Iterator> struct iterator_reference { typedef typename boost::detail::iterator_traits<Iterator>::reference type; };
   } 

   using iterators::iterator_reference;
} 

namespace boost { namespace mpl { namespace aux {

typedef char (&no_tag)[1];
typedef char (&yes_tag)[2];

}}}

namespace mpl_ {

struct failed {};

template< bool C >  struct assert        { typedef void* type; };
template<>          struct assert<false> { typedef assert type; };

template< bool C > int assertion_failed( typename assert<C>::type );

struct assert_ {};

}

namespace boost { namespace mpl { namespace aux {

template< typename T > struct type_wrapper
{
    typedef T type;
};


template< typename T > struct wrapped_type;

template< typename T > struct wrapped_type< type_wrapper<T> >
{
    typedef T type;
};

}}}


namespace boost
{
    namespace range_detail
    {
      template< typename T, typename fallback_ = boost::mpl::bool_<false> > struct has_iterator { struct gcc_3_2_wknd { template< typename U > static boost::mpl::aux::yes_tag test( boost::mpl::aux::type_wrapper<U> const volatile* , boost::mpl::aux::type_wrapper<typename U::iterator> * = 0 ); static boost::mpl::aux::no_tag test(...); }; typedef boost::mpl::aux::type_wrapper<T> t_; static const bool value = sizeof(gcc_3_2_wknd::test(static_cast<t_* >(0))) == sizeof(boost::mpl::aux::yes_tag); typedef boost::mpl::bool_<value> type; }; template< typename C, bool B = has_iterator<C> ::value > struct extract_iterator {}; template< typename C > struct extract_iterator< C, true > { typedef typename C::iterator type; };

      template< typename C > struct range_mutable_iterator : range_detail::extract_iterator< typename remove_reference<C>::type> {};
    } 

template<typename C, typename Enabler=void> struct range_mutable_iterator : range_detail::range_mutable_iterator< typename remove_reference<C>::type > {};
} 

namespace boost
{
    namespace range_detail
    {
      template< typename T, typename fallback_ = boost::mpl::bool_<false> > struct has_const_iterator { struct gcc_3_2_wknd { template< typename U > static boost::mpl::aux::yes_tag test( boost::mpl::aux::type_wrapper<U> const volatile* , boost::mpl::aux::type_wrapper<typename U::const_iterator> * = 0 ); static boost::mpl::aux::no_tag test(...); }; typedef boost::mpl::aux::type_wrapper<T> t_; static const bool value = sizeof(gcc_3_2_wknd::test(static_cast<t_* >(0))) == sizeof(boost::mpl::aux::yes_tag); typedef boost::mpl::bool_<value> type; }; template< typename C, bool B = has_const_iterator<C> ::value > struct extract_const_iterator {}; template< typename C > struct extract_const_iterator< C, true > { typedef typename C::const_iterator type; };

      template< typename C > struct range_const_iterator : extract_const_iterator<C> {};
    } 

template<typename C, typename Enabler=void> struct range_const_iterator : range_detail::range_const_iterator< typename remove_reference<C>::type > {};
} 




namespace Sawyer {

template<class T>
class SharedPointer {

public:
  template<class Y> SharedPointer(const SharedPointer<Y> &other);

  template<class Y> explicit SharedPointer(Y *rawPtr);
};

class  SharedObject { };

} 



enum boost_foreach_argument_dependent_lookup_hack
{
    boost_foreach_argument_dependent_lookup_hack_value
};

namespace boost
{

namespace foreach
{
    typedef boost_foreach_argument_dependent_lookup_hack tag;
    template<typename T>
    struct is_lightweight_proxy;
    template<typename T>
    struct is_noncopyable;
} 

} 



namespace boost
{
template<typename T>
class iterator_range;
template<typename T>
class sub_range;

namespace foreach
{
    template<typename T>
    inline std::pair<T, T> in_range(T begin, T end)
    {
        return std::make_pair(begin, end);
    }
    
    template<typename T>
    struct is_lightweight_proxy
     : boost::mpl::false_
    {
    };

    template<typename T>
    struct is_noncopyable
     : boost::mpl::or_< boost::is_abstract<T>, boost::is_base_and_derived<boost::noncopyable, T> >
    {
    };
} 
}

template<typename T>
inline boost::foreach::is_lightweight_proxy<T> *
boost_foreach_is_lightweight_proxy(T *&, boost::foreach::tag) { return 0; }

template<typename T>
inline boost::mpl::true_ *
boost_foreach_is_lightweight_proxy(std::pair<T, T> *&, boost::foreach::tag) { return 0; }

template<typename T>
inline boost::foreach::is_noncopyable<T> *
boost_foreach_is_noncopyable(T *&, boost::foreach::tag) { return 0; }




namespace boost
{
namespace foreach_detail_
{
template<typename Bool1, typename Bool2>
inline boost::mpl::and_<Bool1, Bool2> *and_(Bool1 *, Bool2 *) { return 0; }

template<typename Bool1, typename Bool2, typename Bool3>
inline boost::mpl::and_<Bool1, Bool2, Bool3> *and_(Bool1 *, Bool2 *, Bool3 *) { return 0; }

template<typename Bool1, typename Bool2>
inline boost::mpl::or_<Bool1, Bool2> *or_(Bool1 *, Bool2 *) { return 0; }

template<typename Bool1, typename Bool2, typename Bool3>
inline boost::mpl::or_<Bool1, Bool2, Bool3> *or_(Bool1 *, Bool2 *, Bool3 *) { return 0; }

template<typename Bool1>
inline boost::mpl::not_<Bool1> *not_(Bool1 *) { return 0; }

template<typename T>
inline boost::is_array<T> *is_array_(T const &) { return 0; }

template<typename T>
inline boost::is_const<T> *is_const_(T &) { return 0; }

struct auto_any_base
{
    operator bool() const
    {
        return false;
    }
};

template<typename T>
struct auto_any : auto_any_base
{
    explicit auto_any(T const &t)
      : item(t)
    {
    }

    mutable T item;
};

typedef auto_any_base const &auto_any_t;

template<typename T, typename C>
inline typename boost::mpl::if_<C, T const, T>::type &auto_any_cast(auto_any_t a)
{
    return static_cast<auto_any<T> const &>(a).item;
}

typedef boost::mpl::true_ const_;



template<typename T, typename C = boost::mpl::false_>
struct type2type
  : boost::mpl::if_<C, T const, T>
{
};

template<typename T>
struct wrap_cstr
{
    typedef T type;
};

template<>
struct wrap_cstr<char *>
{
    typedef wrap_cstr<char *> type;
    typedef char *iterator;
    typedef char *const_iterator;
};

template<typename T>
struct is_char_array
  : mpl::and_<
        is_array<T>
      , mpl::or_<
            is_convertible<T, char const *>
          , is_convertible<T, wchar_t const *>
        >
    >
{};


template<typename T, typename C = boost::mpl::false_>
struct foreach_iterator
{
    
    struct IS_THIS_AN_ARRAY_OR_A_NULL_TERMINATED_STRING; typedef struct IS_THIS_AN_ARRAY_OR_A_NULL_TERMINATED_STRING349 : boost::mpl::assert_ { static boost::mpl::failed * * * * * * * * * * * * (IS_THIS_AN_ARRAY_OR_A_NULL_TERMINATED_STRING:: * * * * * * * * * * * * assert_arg()) (T&) { return 0; } } mpl_assert_arg349; static const std::size_t mpl_assertion_in_line_349 = sizeof( boost::mpl::assertion_failed<((!is_char_array<T> ::value))>( mpl_assert_arg349::assert_arg() ) );

    
    
    typedef typename wrap_cstr<T>::type container;

    typedef typename boost::mpl::eval_if<
        C
      , range_const_iterator<container>
      , range_mutable_iterator<container>
    >::type type;
};



template<typename T, typename C = boost::mpl::false_>
struct foreach_reference
  : iterator_reference<typename foreach_iterator<T, C>::type>
{
};


template<typename T>
inline type2type<T> *encode_type(T &, boost::mpl::false_ *) { return 0; }

template<typename T>
inline type2type<T, const_> *encode_type(T const &, boost::mpl::true_ *) { return 0; }

inline bool set_false(bool &b)
{
    b = false;
    return false;
}

template<typename T>
inline T *&to_ptr(T const &)
{
    static T *t = 0;
    return t;
}

template<typename T>
struct rvalue_probe
{
    rvalue_probe(T &t, bool &b)
      : value(t)
      , is_rvalue(b)
    {
    }

    struct private_type_ {};
    
    typedef typename boost::mpl::if_<
        boost::mpl::or_<boost::is_abstract<T>, boost::is_array<T> >, private_type_, T
    >::type value_type;
    
    operator value_type()
    {
        this->is_rvalue = true;
        return this->value;
    }

    operator T &() const
    {
        return this->value;
    }

private:
    T &value;
    bool &is_rvalue;
};

template<typename T>
rvalue_probe<T> make_probe(T &t, bool &b); // { return rvalue_probe<T>(t, b); }

template<typename T>
rvalue_probe<T const> make_probe(T const &t, bool &b); // { return rvalue_probe<T const>(t, b); }




template<typename T> struct simple_variant {};

inline bool *
should_copy_impl(boost::mpl::false_ *, boost::mpl::false_ *, bool *is_rvalue);
  // { return is_rvalue; }

template<typename T>
inline auto_any<simple_variant<T> >
contain(T const &t, bool *rvalue);
// { return auto_any<simple_variant<T> >(*rvalue ? simple_variant<T>(t) : simple_variant<T>(&t)); }

template<typename T>
inline auto_any<typename foreach_iterator<T, const_>::type>
begin(auto_any_t col, type2type<T, const_> *, bool *);
// { return auto_any<typename foreach_iterator<T, const_>::type>(boost::begin(*auto_any_cast<simple_variant<T>, boost::mpl::false_>(col).get())); }

template<typename T>
inline auto_any<typename foreach_iterator<T, const_>::type>
end(auto_any_t col, type2type<T, const_> *, bool *);
  // { return auto_any<typename foreach_iterator<T, const_>::type>(boost::end(*auto_any_cast<simple_variant<T>, boost::mpl::false_>(col).get())); }

template<typename T, typename C>
inline bool done(auto_any_t cur, auto_any_t end, type2type<T, C> *);
  // { typedef typename foreach_iterator<T, C>::type iter_t; return auto_any_cast<iter_t, boost::mpl::false_>(cur) == auto_any_cast<iter_t, boost::mpl::false_>(end); }

template<typename T, typename C>
inline void next(auto_any_t cur, type2type<T, C> *);
// { typedef typename foreach_iterator<T, C>::type iter_t; ++auto_any_cast<iter_t, boost::mpl::false_>(cur); }

template<typename T, typename C>
inline typename foreach_reference<T, C>::type
deref(auto_any_t cur, type2type<T, C> *);
// { typedef typename foreach_iterator<T, C>::type iter_t; return *auto_any_cast<iter_t, boost::mpl::false_>(cur); }

} 
} 

namespace Sawyer { 
namespace CommandLine {

struct  Location {};
 
extern const Location NOWHERE;

class  ValueSaver: public SharedObject {

protected:
    ValueSaver() {}
public:
    typedef SharedPointer<ValueSaver> Ptr;
};

template<typename T>
class TypedSaver: public ValueSaver {
     T &storage_;
protected:
  TypedSaver(T &storage) : storage_(storage) {}
public:
    typedef SharedPointer<TypedSaver> Ptr;
  static Ptr instance(T &storage) { return Ptr(new TypedSaver(storage)); }
  virtual void save(const boost::any &value) const
  { storage_ = boost::any_cast<T>(value); }
};


class  ParsedValue {

public:
     
    ParsedValue(const boost::any value, const Location &loc, const std::string &str, const ValueSaver::Ptr &saver);
};

 
class  Switch {
public:

#if 1
    template<typename T>
    Switch& intrinsicValue(const T &value, T &storage) {
        return *this;
    }
#endif

#if 1
    Switch& intrinsicValue(const char *value, std::string &storage) {
        ParsedValue(std::string(value), NOWHERE, value, TypedSaver<std::string>::instance(storage));
        return *this;
    }
#endif     
};


} 
} 


#if 1


namespace Sawyer {
namespace Container { 
namespace BitVectorSupport {
 
template<class T> struct RemoveConst { typedef T Base; };
template<class T> struct RemoveConst<const T> { typedef T Base; };

  // typedef Interval<size_t> BitRange;
 
struct LowToHigh {};
struct HighToLow {};


#if 1
 
template<class Word>
void fromDecimal(Word *vec, const std::string &input) 
   {
     boost::uint64_t v = 0;

#if 1
     if (bool _foreach_is_rvalue1401 = false) {} else if (boost::foreach_detail_::auto_any_t _foreach_col1401 = boost::foreach_detail_::contain( (true ? boost::foreach_detail_::make_probe((input), _foreach_is_rvalue1401) : (input)) , (boost::foreach_detail_::should_copy_impl( true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::is_array_(input) , boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value) , boost::foreach_detail_::not_(boost::foreach_detail_::is_const_(input))) , true ? 0 : boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , &_foreach_is_rvalue1401)))) {} else if (boost::foreach_detail_::auto_any_t _foreach_cur1401 = boost::foreach_detail_::begin( _foreach_col1401 , (true ? 0 : boost::foreach_detail_::encode_type(input, boost::foreach_detail_::is_const_(input))) , (boost::foreach_detail_::should_copy_impl( true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::is_array_(input) , boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value) , boost::foreach_detail_::not_(boost::foreach_detail_::is_const_(input))) , true ? 0 : boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , &_foreach_is_rvalue1401)))) {} else if (boost::foreach_detail_::auto_any_t _foreach_end1401 = boost::foreach_detail_::end( _foreach_col1401 , (true ? 0 : boost::foreach_detail_::encode_type(input, boost::foreach_detail_::is_const_(input))) , (boost::foreach_detail_::should_copy_impl( true ? 0 : boost::foreach_detail_::or_( boost::foreach_detail_::is_array_(input) , boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value) , boost::foreach_detail_::not_(boost::foreach_detail_::is_const_(input))) , true ? 0 : boost::foreach_detail_::and_( boost::foreach_detail_::not_(boost_foreach_is_noncopyable( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , boost_foreach_is_lightweight_proxy( boost::foreach_detail_::to_ptr(input) , boost_foreach_argument_dependent_lookup_hack_value)) , &_foreach_is_rvalue1401)))) {} else for (bool _foreach_continue1401 = true; _foreach_continue1401 && !boost::foreach_detail_::done( _foreach_cur1401 , _foreach_end1401 , (true ? 0 : boost::foreach_detail_::encode_type(input, boost::foreach_detail_::is_const_(input)))); _foreach_continue1401 ? boost::foreach_detail_::next( _foreach_cur1401 , (true ? 0 : boost::foreach_detail_::encode_type(input, boost::foreach_detail_::is_const_(input)))) : (void)0) if (boost::foreach_detail_::set_false(_foreach_continue1401)) {} else for (char ch = boost::foreach_detail_::deref( _foreach_cur1401 , (true ? 0 : boost::foreach_detail_::encode_type(input, boost::foreach_detail_::is_const_(input)))); !_foreach_continue1401; _foreach_continue1401 = true) {

    }
#endif

}

#endif


} 
} 
} 


#endif















