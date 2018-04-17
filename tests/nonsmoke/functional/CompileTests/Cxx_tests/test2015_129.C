
#if 1
namespace std
{
  typedef long unsigned int 	size_t;
  typedef long int	ptrdiff_t;

}

typedef long int ptrdiff_t;

typedef long unsigned int size_t;

#endif


#if 1

namespace boost{
   __extension__ typedef long long long_long_type;
   __extension__ typedef unsigned long long ulong_long_type;
}

namespace boost{
   __extension__ typedef __int128 int128_type;
   __extension__ typedef unsigned __int128 uint128_type;
}


#endif


#if 1

namespace std 
{
  struct input_iterator_tag { };
// struct output_iterator_tag { };
  struct forward_iterator_tag : public input_iterator_tag { };
  struct bidirectional_iterator_tag : public forward_iterator_tag { };
  struct random_access_iterator_tag : public bidirectional_iterator_tag { };
#if 1
  template<typename _Iterator>
    struct iterator_traits
    {
      typedef typename _Iterator::iterator_category iterator_category;
      typedef typename _Iterator::value_type        value_type;
      typedef typename _Iterator::difference_type   difference_type;
      typedef typename _Iterator::pointer           pointer;
      typedef typename _Iterator::reference         reference;
    };
#endif
#if 1
  template<typename _Tp>
    struct iterator_traits<_Tp*>
    {
      typedef random_access_iterator_tag iterator_category;
      typedef _Tp                         value_type;
      typedef ptrdiff_t                   difference_type;
      typedef _Tp*                        pointer;
      typedef _Tp&                        reference;
    };
#endif
} 
#endif



#if 1

namespace mpl_ { namespace aux {} }
namespace boost { namespace mpl { using namespace mpl_; 
namespace aux { using namespace mpl_::aux; }
}}

namespace mpl_ {

struct void_;

}
namespace boost { namespace mpl { using ::mpl_::void_; } }

namespace mpl_ {

template< bool C_ > struct bool_;


typedef bool_<true> true_;
typedef bool_<false> false_;

}

namespace boost { namespace mpl { using ::mpl_::bool_; } }
namespace boost { namespace mpl { using ::mpl_::true_; } }
namespace boost { namespace mpl { using ::mpl_::false_; } }


namespace mpl_ {
struct integral_c_tag { static const int value = 0; };
}
namespace boost { namespace mpl { using ::mpl_::integral_c_tag; } }


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
struct na
{
    typedef na type;
    enum { value = 0 };
};
}
namespace boost { namespace mpl { using ::mpl_::na; } }

namespace boost { namespace mpl { namespace aux {

template< typename F > struct template_arity;

}}}


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




namespace boost {
namespace type_traits {
template <bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false> struct ice_or;
} 
} 

namespace boost {
namespace type_traits {
template <bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true> struct ice_and;
} 
} 

namespace boost {
namespace type_traits {

template <bool b>
struct ice_not
{
    static const bool value = true;
};
} 
} 


#if 1
namespace boost {
namespace detail {
template <typename T> struct cv_traits_imp {};
} 
} 
#endif

namespace boost {
namespace detail{
template <class T>
struct rvalue_ref_filter_rem_cv
{
   typedef typename boost::detail::cv_traits_imp<T*>::unqualified_type type;
};
}

template< typename T > struct remove_cv { public: typedef typename boost::detail::rvalue_ref_filter_rem_cv<T> ::type type;  }; 
template< typename T > struct remove_cv<T& > { public: typedef T& type; };
} 

namespace mpl_ { template< typename T, T N > struct integral_c; }
namespace boost { namespace mpl { using ::mpl_::integral_c; } }

namespace mpl_ {

template< typename T, T N >
struct integral_c
{
    static const T value = N;
};

template< typename T, T N > T const integral_c< T, N >::value;
}



namespace mpl_ {
template< bool C > struct integral_c<bool, C> {};
}

#endif

#if 1

namespace boost{

template <class T, T val>
struct integral_constant // : public mpl::integral_c<T, val>
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
#endif



namespace boost {
namespace detail {
template<class T, T min_val, T max_val> class integer_traits_base {};
} 
} 

namespace boost {
template< typename T, typename U > struct is_same : public ::boost::integral_constant<bool,false> { public:   }; 
template< typename T > struct is_same< T,T > : public ::boost::integral_constant<bool,true> { public:   };
} 

namespace boost {
namespace detail{
template <class T>
struct is_volatile_rval_filter
{
  static const bool value = false;
};
}
  template< typename T > struct is_volatile {};
  template< typename T > struct is_volatile< T& > {};
}

namespace boost { namespace detail {
template <class Iterator>
struct iterator_traits : std::iterator_traits<Iterator> {};
}}

namespace boost {
  template< typename T > struct is_integral {};
}

namespace boost {
  template< typename T > struct is_enum {};
} 

namespace boost {
  template< typename T > struct is_unsigned {};
} 

namespace boost {
namespace detail{

template <class T>
struct is_const_rvalue_filter
   {
     static const bool value = false;
   };
}


template< typename T > struct is_const {};
template< typename T > struct is_const< T& > {};
} 

namespace boost {
template< typename T > struct add_const { public: typedef T const type;  }; 
} 

namespace boost {
template< typename T > struct add_volatile { public: typedef T volatile type;  }; 
} 

namespace boost {
namespace detail {

template <class T>
struct make_unsigned_imp
{
   typedef typename remove_cv<T>::type t_no_cv;
   typedef typename mpl::if_c<
      (::boost::type_traits::ice_and< 
         ::boost::is_unsigned<T>::value,
         ::boost::is_integral<T>::value,
         ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, char>::value>::value,
         ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, wchar_t>::value>::value,
         ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, bool>::value>::value >::value),
      T,
      typename mpl::if_c<
         (::boost::type_traits::ice_and< 
            ::boost::is_integral<T>::value,
            ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, char>::value>::value,
            ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, wchar_t>::value>::value,
            ::boost::type_traits::ice_not< ::boost::is_same<t_no_cv, bool>::value>::value>
         ::value),
         typename mpl::if_<
            is_same<t_no_cv, signed char>,
            unsigned char,
            typename mpl::if_<
               is_same<t_no_cv, short>,
               unsigned short,
               typename mpl::if_<
                  is_same<t_no_cv, int>,
                  unsigned int,
                  typename mpl::if_<
                     is_same<t_no_cv, long>,
                     unsigned long,
                     typename mpl::if_c<
                        sizeof(t_no_cv) == sizeof(boost::ulong_long_type), 
                        boost::ulong_long_type, 
                        boost::uint128_type
                     >::type
                  >::type
               >::type
            >::type
         >::type,
         
         typename mpl::if_c<
            sizeof(t_no_cv) == sizeof(unsigned char),
            unsigned char,
            typename mpl::if_c<
               sizeof(t_no_cv) == sizeof(unsigned short),
               unsigned short,
               typename mpl::if_c<
                  sizeof(t_no_cv) == sizeof(unsigned int),
                  unsigned int,
                  typename mpl::if_c<
                     sizeof(t_no_cv) == sizeof(unsigned long),
                     unsigned long,
                     typename mpl::if_c<
                        sizeof(t_no_cv) == sizeof(boost::ulong_long_type), 
                        boost::ulong_long_type, 
                        boost::uint128_type
                     >::type
                  >::type
               >::type
            >::type
         >::type
      >::type
   >::type base_integer_type;

   typedef typename mpl::if_< is_const<T>, typename add_const<base_integer_type>::type, base_integer_type >::type const_base_integer_type;
   typedef typename mpl::if_< is_volatile<T>, typename add_volatile<const_base_integer_type>::type, const_base_integer_type >::type type;
};
} 
} 


namespace boost {
  // template< typename T > struct is_array : public ::boost::integral_constant<bool,false> { public:   }; 
  // template< typename T > struct is_array { typedef T type; } // : public ::boost::integral_constant<bool,false> { public:   }; 
     template< typename T > struct is_array : public ::boost::integral_constant<bool,false> { public:   }; 

  // template< typename T > struct is_abstract : public ::boost::integral_constant<bool,::boost::detail::is_abstract_imp<T> ::value> { public:   }; 
     template< typename T > struct is_abstract : public ::boost::integral_constant<bool,false> { public:   }; 

  // Template classes can't be differentiated based on the base class signature (that would be wierd).
  // template< typename T > struct is_abstract : public ::boost::integral_constant<bool,true> { public:   }; 
  // template< typename T > struct is_abstract { public:   }; 

} 


namespace boost { 
namespace mpl {
namespace aux {

template< bool C_, typename T1, typename T2, typename T3, typename T4 >
struct or_impl : true_
{};

template< typename T1, typename T2, typename T3, typename T4 >
struct or_impl< false,T1,T2,T3,T4 > : or_impl< T1::type::value, T2, T3, T4, false_ >
{};

  template<> struct or_impl< false, false_, false_, false_, false_ > : false_ { };
} 

template< typename T1 = na, typename T2 = na, typename T3 = false_, typename T4 = false_, typename T5 = false_ > 
struct or_ : aux::or_impl< T1::type::value, T2, T3, T4, T5 > { };
}}

namespace boost {
template <class Iterator>
struct iterator_value
{
    typedef typename boost::detail::iterator_traits<Iterator>::value_type type;
};
  
template <class Iterator>
struct iterator_reference
{
    typedef typename boost::detail::iterator_traits<Iterator>::reference type;
};
} 

namespace boost
{
        template<class IteratorT>
        class iterator_range
        {
          public:
            typedef typename iterator_value<IteratorT>::type value_type;
            typedef typename iterator_reference<IteratorT>::type reference;
            typedef IteratorT iterator;

        private: 
         // typedef typename boost::mpl::if_< boost::mpl::or_< boost::is_abstract< value_type >, boost::is_array< value_type > >, reference, value_type >::type abstract_value_type;
         // typedef typename boost::mpl::if_< boost::mpl::or_< boost::is_array< value_type >, boost::is_array< value_type > >, reference, value_type >::type abstract_value_type;
            typedef typename boost::mpl::if_< boost::mpl::or_< boost::is_array< value_type >, boost::is_array< value_type > >, typename iterator_value<IteratorT>::type, typename iterator_reference<IteratorT>::type >::type abstract_value_type;

        public:
            template< class Iterator >
            iterator_range( Iterator Begin, Iterator End )
            {}

            IteratorT begin() const
            {
                return m_Begin;
            }

        private:
            IteratorT m_Begin;
        };
}


namespace boost {
    namespace detail 
    {
        template< class CharT , class Traits , bool RequiresStringbuffer >
        class lexical_stream_limited_src
        {
            bool operator<<(const iterator_range<const signed char*>& rng) 
            {
                return (*this) << iterator_range<char*>(
                    const_cast<char*>(reinterpret_cast<const char*>(rng.begin())),
                    const_cast<char*>(reinterpret_cast<const char*>(rng.begin()))
                );
            }
        };
    }


void foobar()
   {
     const char* chars;
     ::boost::iterator_range<const char*>(chars, chars);
   }
} 

