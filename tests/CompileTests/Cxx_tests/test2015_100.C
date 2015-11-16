 
namespace std { typedef long unsigned int size_t; }

typedef long unsigned int size_t;

namespace std { template<typename> class allocator; } 

namespace std 
{
  template<class _CharT> struct char_traits;
  template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;
  typedef basic_string<char> string;   
} 

namespace std { template<class _CharT> struct char_traits {}; } 

namespace std 
{
  template<typename _CharT, typename _Traits = char_traits<_CharT> > class basic_ostream;
  template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_ostringstream;
} 

namespace std { template<typename _CharT, typename _Traits, typename _Alloc> class basic_string {}; } 

namespace mpl_ { struct void_; }
namespace boost { namespace mpl { using ::mpl_::void_; } }

namespace mpl_ {
   template< bool C_ > struct bool_;
   typedef bool_<true> true_;
   typedef bool_<false> false_;
}

namespace boost { namespace mpl { using ::mpl_::bool_; } }
namespace boost { namespace mpl { using ::mpl_::true_; } }
namespace boost { namespace mpl { using ::mpl_::false_; } }

namespace mpl_ { struct integral_c_tag { static const int value = 0; }; }
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

   template< bool C_ > bool const bool_<C_>::value;
}

namespace mpl_ {
   struct na
      {
        typedef na type;
        enum { value = 0 };
      };
}

namespace boost { namespace mpl { using ::mpl_::na; } }
namespace boost { namespace mpl {
   template< typename T > struct is_na : false_ {};
   template<> struct is_na<na> : true_ {};
   template< typename T > struct is_not_na : true_ {};
   template<> struct is_not_na<na> : false_ {};
   template< typename T, typename U > struct if_na { typedef T type; };
   template< typename U > struct if_na<na,U> { typedef U type; };
}}


namespace mpl_ { template< int N > struct int_; }
namespace boost { namespace mpl { using ::mpl_::int_; } }

namespace mpl_ {
template< int N >
struct int_
   {
     static const int value = N;
     typedef int_ type;
     typedef int value_type;
     typedef integral_c_tag tag;
     typedef mpl_::int_< static_cast<int>((value + 1)) > next;
     typedef mpl_::int_< static_cast<int>((value - 1)) > prior;
    
     operator int() const { return static_cast<int>(this->value); } 
   };

template< int N > int const mpl_::int_< N > ::value;
}

namespace boost { namespace mpl { namespace aux { template< typename F > struct template_arity; } } }
namespace boost { namespace mpl { template< typename T = na, typename Tag = void_ > struct lambda; } }


namespace boost { namespace mpl {

template< bool C, typename T1, typename T2 > struct if_c { typedef T1 type; };

template< typename T1, typename T2 > struct if_c<false,T1,T2> { typedef T2 type; };

template< typename T1 = na, typename T2 = na, typename T3 = na >
struct if_
   {
     private:
          typedef if_c< static_cast<bool>(T1::value), T2, T3 > almost_type_;

     public:
          typedef typename almost_type_::type type;  
   };

template< > 
struct if_<  na , na , na > 
   {
     template<  typename T1, typename T2, typename T3, typename T4 = na, typename T5 = na >
     struct apply : if_<  T1 , T2 , T3 > { }; 
   };

template< typename Tag > 
struct lambda< if_<  na , na , na > , Tag  > 
   {
     typedef false_ is_le;
     typedef if_<  na , na , na > result_;
     typedef if_<  na , na , na > type;
   };
}}

namespace boost {
namespace type_traits {
   typedef char yes_type;
   struct no_type { char padding[8]; };
} 
} 

namespace boost {
namespace type_traits {

template < bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false > struct ice_or;

template <bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7>
struct ice_or
   {
     static const bool value = true;
   };

template <>
struct ice_or< false, false, false, false, false, false, false >
   {
     static const bool value = false;
   };
} 
} 

namespace boost {
namespace type_traits {

template < bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true >
struct ice_and;

template < bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7 >
struct ice_and
   {
     static const bool value = false;
   };

template <>
struct ice_and<true, true, true, true, true, true, true>
   {
     static const bool value = true;
   };

} } 

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

template <typename T> struct cv_traits_imp {};

template <typename T>
struct cv_traits_imp<T*>
   {
     static const bool is_const = false;
     static const bool is_volatile = false;
     typedef T unqualified_type;
   };

template <typename T>
struct cv_traits_imp<const T*>
   {
     static const bool is_const = true;
     static const bool is_volatile = false;
     typedef T unqualified_type;
   };

template <typename T>
struct cv_traits_imp<volatile T*>
   {
     static const bool is_const = false;
     static const bool is_volatile = true;
     typedef T unqualified_type;
   };

template <typename T>
struct cv_traits_imp<const volatile T*>
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
struct rvalue_ref_filter_rem_cv
   {
     typedef typename boost::detail::cv_traits_imp<T*>::unqualified_type type;
   };
}

template< typename T > struct remove_cv { public: typedef typename boost::detail::rvalue_ref_filter_rem_cv<T> ::type type;  }; 
template< typename T > struct remove_cv<T& > { public: typedef T& type; };
template< typename T, std::size_t N > struct remove_cv<T const[N]> { public: typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T volatile[N]> { public: typedef T type[N]; };
template< typename T, std::size_t N > struct remove_cv<T const volatile[N]> { public: typedef T type[N]; };
} 


namespace mpl_ { template< typename T, T N > struct integral_c; }
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

template< typename T, T N > T const integral_c< T, N > ::value;
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

namespace detail { template <typename T> struct is_fundamental_impl : public ::boost::type_traits::ice_or< false, false > {}; } 

template< typename T > struct is_fundamental : public ::boost::integral_constant<bool,::boost::detail::is_fundamental_impl<T> ::value> { public:   }; 

} 

namespace boost {
namespace detail{ template <class T> struct remove_rvalue_ref { typedef T type; }; } 

template< typename T > struct remove_reference { public: typedef typename boost::detail::remove_rvalue_ref<T> ::type type;  }; 
template< typename T > struct remove_reference<T& > { public: typedef T type; };
}


namespace boost {
namespace detail {
namespace has_left_shift_impl {

struct dont_care { };

template < typename Lhs, typename Rhs, typename Ret, bool Forbidden_if >
struct trait_impl1;

template < typename Lhs, typename Rhs, typename Ret >
struct trait_impl1 < Lhs, Rhs, Ret, false > {
   static const bool value = false;
};

template < typename Lhs, typename Rhs, typename Ret >
struct trait_impl 
   {
     typedef typename ::boost::remove_reference<Lhs>::type Lhs_noref;
     typedef typename ::boost::remove_reference<Rhs>::type Rhs_noref;
     typedef typename ::boost::remove_cv<Lhs_noref>::type Lhs_nocv;
     typedef typename ::boost::remove_cv<Rhs_noref>::type Rhs_nocv;

  // This statement (simplified) is required to demonstrate the bug.
     static const bool value = (trait_impl1 < Lhs_noref, Rhs_noref, Ret, ::boost::type_traits::ice_or< false , false , ::boost::type_traits::ice_and< ::boost::is_fundamental< Rhs_nocv > ::value , false > ::value, false > ::value > ::value);
   };
} 
}

  // This statement is required to demonstrate the bug.
template< typename Lhs, typename Rhs=Lhs, typename Ret= ::boost::detail::has_left_shift_impl::dont_care > struct has_left_shift : public ::boost::integral_constant<bool,(::boost::detail::has_left_shift_impl::trait_impl < Lhs, Rhs, Ret > ::value)> { public:   }; 
}


namespace boost { namespace container { template <class CharT, class Traits = std::char_traits<CharT>, class Allocator  = std::allocator<CharT> > class basic_string; } }  

namespace boost {
   namespace detail { template < typename T > struct is_char_or_wchar { typedef unsigned char result_type; static const bool value = false; }; }

   namespace detail { template < class T > struct deduce_character_type_later {}; }

   namespace detail { template < typename Type > struct stream_char_common: public boost::mpl::if_c< false, Type, boost::detail::deduce_character_type_later< Type > > {}; }

   namespace detail {
      // This template is required to be the primary template (see EDG error message if not present).
     template < class Char > struct deduce_source_char_impl { typedef Char type; };

     template < class T > 
     struct deduce_source_char_impl< deduce_character_type_later< T > > 
        {
          typedef boost::has_left_shift< std::basic_ostream< char >, T > result_t;
          typedef typename boost::mpl::if_c< result_t::value, char, wchar_t >::type type;
        };
   }

   namespace detail {
     template < class T >
     struct deduce_source_char
        {
         // We need this to reproduce the bug.
            typedef typename stream_char_common< T >::type stage1_type;
            typedef typename deduce_source_char_impl< stage1_type >::type stage2_type;
        };
   }

   namespace detail { template< class Source > struct lcast_src_length { static const std::size_t value = 1; }; }

   namespace detail {
     template <class Source, class Target>
     struct lexical_cast_stream_traits 
        {
          typedef Source src;

       // We need this to reproduce the bug.
          typedef typename boost::remove_cv<src>::type no_cv_src;

       // We need this to reproduce the bug.
          typedef boost::detail::deduce_source_char<no_cv_src>                           deduce_src_char_metafunc;
          typedef char src_char_t;

          typedef char target_char_t;

          typedef char char_type;

          typedef std::char_traits< char_type > traits;

       // This statement is required to reproduce the bug.
          typedef boost::type_traits::ice_not< boost::type_traits::ice_or< false, false, boost::detail::is_char_or_wchar< typename deduce_src_char_metafunc::stage1_type >::value >::value > is_source_input_not_optimized_t;
          static const bool requires_stringbuf = false;
            
          typedef boost::detail::lcast_src_length<no_cv_src> len_t;
        };
   }

   namespace detail {
     template< class CharT, class Traits, bool RequiresStringbuffer >
     class lexical_stream_limited_src
        {

          typedef std::basic_ostringstream<CharT, Traits>                 out_stream_t;
          typedef typename out_stream_t::type deduced_out_stream_t;

          CharT* start;
          CharT* finish;
          deduced_out_stream_t out_stream;
        };
   }

   namespace detail {
     template<typename Target, typename Source>
     struct lexical_cast_do_cast
        {
          static inline Target lexical_cast_impl(const Source& arg)
             {
               typedef lexical_cast_stream_traits<Source, Target>  stream_trait;

            // This typedef is required to demonstrate the recursive call bug to the mangled name generation.
               typedef detail::lexical_stream_limited_src< typename stream_trait::char_type, typename stream_trait::traits, stream_trait::requires_stringbuf > interpreter_type;

               Target result;

               return result;
             }
        };
   }

     template <typename Target, typename Source>
     void lexical_cast(const Source &arg)
        {
          typedef typename boost::detail::lexical_cast_do_cast<Target, Source > caster_type;

          caster_type::lexical_cast_impl(arg);
        }
} 

void foobar() 
   {
     size_t idx;
     boost::lexical_cast<std::string>(idx);
   } 
