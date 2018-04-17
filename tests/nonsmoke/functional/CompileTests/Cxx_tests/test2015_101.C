 
namespace std { typedef long unsigned int size_t; }

namespace std { template<typename> class allocator; } 

namespace std 
{
  template<class _CharT> struct char_traits;
  template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;
  typedef basic_string<char> string;   
} 

namespace std { template<class _CharT> struct char_traits {}; }
namespace std { template<typename _CharT, typename _Traits = char_traits<_CharT> > class basic_ostream; }
namespace std { template<typename _CharT, typename _Traits, typename _Alloc> class basic_string {}; }

namespace boost { namespace mpl {
   template< bool C, typename T1, typename T2 > struct if_c { typedef T1 type; };
   template< typename T1, typename T2 > struct if_c<false,T1,T2> { typedef T2 type; };
}}

namespace boost {
namespace type_traits {
   template < bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false > struct ice_or;
   template <> struct ice_or< false, false, false, false, false, false, false > { static const bool value = false; };
} 
} 

namespace boost {
namespace type_traits {
   template < bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true > struct ice_and;
   template < bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7 > struct ice_and { static const bool value = false; };
} } 


namespace boost { namespace type_traits { template <bool b> struct ice_not { static const bool value = true; }; } } 

namespace mpl_ { template< typename T, T N > struct integral_c; }
namespace boost { namespace mpl { using ::mpl_::integral_c; } }

namespace mpl_ { template< bool C > struct integral_c<bool, C> { static const bool value = C; }; }
namespace boost{ template <class T, T val> struct integral_constant : public mpl::integral_c<T, val> { typedef integral_constant<T,val> type; }; }

namespace boost {
namespace detail { 
   template <typename T> struct is_fundamental_impl : public ::boost::type_traits::ice_or< false, false > {}; }
   template< typename T > struct is_fundamental : public ::boost::integral_constant<bool,::boost::detail::is_fundamental_impl<T> ::value> { public:   }; 
} 

namespace boost {
namespace detail {
namespace has_left_shift_impl {

struct dont_care {};

template < typename Lhs, typename Rhs, typename Ret, bool Forbidden_if > struct trait_impl1;

template < typename Lhs, typename Rhs, typename Ret > struct trait_impl1 < Lhs, Rhs, Ret, false > { static const bool value = false; };

template < typename Lhs, typename Rhs, typename Ret >
struct trait_impl
   {
     typedef Rhs Rhs_nocv;

  // This statement (simplified) is required to demonstrate the bug.
     static const bool value = (trait_impl1< Lhs,Rhs,Ret,::boost::type_traits::ice_or< false,false,::boost::type_traits::ice_and< ::boost::is_fundamental< Rhs_nocv >::value,false >::value,false >::value >::value);
   };
} 
}

  // This statement is required to demonstrate the bug.
template< typename Lhs, typename Rhs=Lhs, typename Ret = ::boost::detail::has_left_shift_impl::dont_care > 
struct has_left_shift : public ::boost::integral_constant< bool,(::boost::detail::has_left_shift_impl::trait_impl < Lhs, Rhs, Ret > ::value)> {}; 
}


namespace boost { namespace container { template <class CharT, class Traits = std::char_traits<CharT>, class Allocator  = std::allocator<CharT> > class basic_string; } }

namespace boost {
   namespace detail { template < typename T > struct is_char_or_wchar { static const bool value = false; }; }

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

   namespace detail {
     template <class Source, class Target>
     struct lexical_cast_stream_traits 
        {
       // We need this to reproduce the bug.
          typedef Source no_cv_src;

       // We need this to reproduce the bug.
          typedef boost::type_traits::ice_not< boost::type_traits::ice_or< false, false, boost::detail::is_char_or_wchar< typename boost::detail::deduce_source_char<no_cv_src>::stage1_type >::value >::value > is_source_input_not_optimized_t;

          static const bool requires_stringbuf = false;
        };
   }

   namespace detail { template< class CharT, class Traits, bool RequiresStringbuffer > class lexical_stream_limited_src {}; }

   namespace detail {
     template<typename Target, typename Source>
     struct lexical_cast_do_cast
        {
          static void lexical_cast_impl(const Source& arg)
             {
               lexical_cast_stream_traits<Source, Target>::requires_stringbuf;
             }
        };
   }

     template <typename Target, typename Source>
     void lexical_cast(const Source &arg)
        {
          boost::detail::lexical_cast_do_cast<Target, Source >::lexical_cast_impl(arg);
        }
}

void foobar() 
   {
     std::size_t idx;
     boost::lexical_cast<std::string>(idx);
   } 
