typedef long unsigned int size_t;
template<typename> class allocator;
template<class _CharT> struct char_traits;
template<typename _CharT, typename _Traits = char_traits<_CharT>, typename _Alloc = allocator<_CharT> > class basic_string;
typedef basic_string<char> string;   
template<class _CharT> struct char_traits {};
template<typename _CharT, typename _Traits = char_traits<_CharT> > class basic_ostream;
template<typename _CharT, typename _Traits, typename _Alloc> class basic_string {};

namespace boost { 
   namespace mpl {
      template< bool C, typename T1, typename T2 > struct if_c { typedef T1 type; };
      template< typename T1, typename T2 > struct if_c<false,T1,T2> { typedef T2 type; };
   }
   namespace type_traits {
      template < bool b1, bool b2, bool b3 = false, bool b4 = false, bool b5 = false, bool b6 = false, bool b7 = false > struct ice_or;
      template <> struct ice_or< false, false, false, false, false, false, false > { static const bool value = false; };
      template < bool b1, bool b2, bool b3 = true, bool b4 = true, bool b5 = true, bool b6 = true, bool b7 = true > struct ice_and;
      template < bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7 > struct ice_and { static const bool value = false; };
      template <bool b> struct ice_not { static const bool value = true; }; }
   }

namespace mpl_ { template< typename T, T N > struct integral_c; }

namespace boost { namespace mpl { using ::mpl_::integral_c; } }

namespace mpl_ { template< bool C > struct integral_c<bool, C> { static const bool value = C; }; }

namespace boost{ 
   template <class T, T val> struct integral_constant : public mpl::integral_c<T, val> { typedef integral_constant<T,val> type; }; 
   namespace detail { template <typename T> struct is_fundamental_impl : public ::boost::type_traits::ice_or< false, false > {}; }
   template< typename T > struct is_fundamental : public ::boost::integral_constant<bool,::boost::detail::is_fundamental_impl<T> ::value> { public:   }; 
   namespace detail {
      namespace has_left_shift_impl {
         struct dont_care {};
         template < typename Lhs, typename Rhs, typename Ret, bool Forbidden_if > struct trait_impl1;
         template < typename Lhs, typename Rhs, typename Ret > struct trait_impl1 < Lhs, Rhs, Ret, false > { static const bool value = false; };
         template < typename Lhs, typename Rhs, typename Ret >
         struct trait_impl
            {
              typedef Rhs Rhs_nocv;
           // static const bool value = (trait_impl1< Lhs,Rhs,Ret,::boost::type_traits::ice_or< false,false,::boost::type_traits::ice_and< ::boost::is_fundamental< Rhs_nocv >::value,false >::value,false >::value >::value);
              static const bool value = (trait_impl1< Lhs,Rhs,Ret,::boost::type_traits::ice_or< false,false,::boost::type_traits::ice_and< ::boost::is_fundamental< Rhs_nocv >::value,false >::value,false >::value >::value);
            };
      } 
   }

template< typename Lhs, typename Rhs=Lhs, typename Ret = ::boost::detail::has_left_shift_impl::dont_care > 
struct has_left_shift : public ::boost::integral_constant< bool,(::boost::detail::has_left_shift_impl::trait_impl < Lhs, Rhs, Ret > ::value)> {};

namespace container { template <class CharT, class Traits = char_traits<CharT>, class Allocator  = allocator<CharT> > class basic_string; } 

   namespace detail { 
     template < typename T > struct is_char_or_wchar { static const bool value = false; }; 
     template < class T > struct deduce_character_type_later {}; 
     template < typename Type > struct stream_char_common: public boost::mpl::if_c< false, Type, boost::detail::deduce_character_type_later< Type > > {}; 
     template < class Char > struct deduce_source_char_impl { typedef Char type; };

     template < class T > 
     struct deduce_source_char_impl< deduce_character_type_later< T > > 
        {
          typedef boost::has_left_shift< basic_ostream< char >, T > result_t;
          typedef typename boost::mpl::if_c< result_t::value, char, wchar_t >::type type;
        };
     template < class T >
     struct deduce_source_char
        {
          typedef typename stream_char_common< T >::type stage1_type;
          typedef typename deduce_source_char_impl< stage1_type >::type stage2_type;
        };
     template <class Source, class Target>
     struct lexical_cast_stream_traits 
        {
          typedef Source no_cv_src;
          typedef boost::type_traits::ice_not< boost::type_traits::ice_or< false, false, boost::detail::is_char_or_wchar< typename boost::detail::deduce_source_char<no_cv_src>::stage1_type >::value >::value > is_source_input_not_optimized_t;

          static const bool requires_stringbuf = false;
        };
     template< class CharT, class Traits, bool RequiresStringbuffer > class lexical_stream_limited_src {}; 
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
     size_t idx;
     boost::lexical_cast<string>(idx);
   }
