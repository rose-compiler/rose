namespace std { typedef long unsigned int size_t; }
namespace std { template<typename _CharT > class basic_string; typedef basic_string<char> string; }
namespace std { template<class _CharT> struct char_traits {}; }
namespace std { template<typename _CharT > class basic_ostream; }
namespace std { template<typename _CharT > class basic_string {}; }

namespace boost { 
   namespace mpl {
      template< bool C, typename T1, typename T2 > struct if_c { typedef T1 type; };
      template< typename T1, typename T2 > struct if_c<false,T1,T2> { typedef T2 type; };
   }

   template< typename T > struct remove_cv { public: typedef T& type; }; 
   template< typename T > struct remove_cv<T& > { public: typedef T& type; };

   template <class T, T val>
   struct integral_constant
      {
        typedef integral_constant<T,val> type;
        static const bool value = val;
      };

   namespace detail { template <typename T> struct is_fundamental_impl { static const bool value = false; }; } 
   template< typename T > struct is_fundamental : public ::boost::integral_constant<bool,::boost::detail::is_fundamental_impl<T> ::value> { public: }; 

   struct dont_care { };
   namespace detail {
      namespace has_left_shift_impl {
         template < typename Lhs, typename Rhs, typename Ret, bool Forbidden_if > struct trait_impl1;
         template < typename Lhs, typename Rhs, typename Ret > struct trait_impl1 < Lhs, Rhs, Ret, false > { static const bool value = false; };

         template < typename Lhs, typename Rhs, typename Ret >
         struct trait_impl 
            {
           // This type evaluates to be: ::boost::detail::has_left_shift_impl::trait_impl < basic_ostream< char > , Rhs_nocv , dont_care > ::Rhs_nocv
              typedef typename ::boost::remove_cv<Rhs>::type Rhs_nocv;

           // This statement (simplified) is required to demonstrate the bug.
              static const bool value = (trait_impl1 < Lhs, Rhs, Ret, ::boost::is_fundamental< Rhs_nocv > ::value > ::value);
            };
      } 
   }

// This statement is required to demonstrate the bug.
   template< typename Lhs, typename Rhs, typename Ret = dont_care > 
   struct has_left_shift : public integral_constant<bool,(detail::has_left_shift_impl::trait_impl<Lhs,Rhs,Ret>::value)> 
      { }; 

   namespace detail { 
     // This instantiates to be used as: "deduce_character_type_later < Rhs_nocv >"
        template < class T > struct deduce_character_type_later {};

     // template < typename Type > struct stream_char_common: public boost::mpl::if_c< false, Type, boost::detail::deduce_character_type_later< Type > > {};
     // template < typename Type > struct stream_char_common: public boost::mpl::if_c< false, Type, Type /* boost::detail::deduce_character_type_later< Type > */ > {};
        template < typename Type > struct stream_char_common: public mpl::if_c< false, Type, deduce_character_type_later< Type > > {};

     // This template is required to be the primary template (see EDG error message if not present).
        template < class Char > struct deduce_source_char_impl { typedef Char type; };

        template < class T > 
        struct deduce_source_char_impl< deduce_character_type_later< T > > 
           {
             typedef boost::has_left_shift< std::basic_ostream< char >, T > result_t;
             typedef typename boost::mpl::if_c< result_t::value, char, char >::type type;
           };
        template < class T >
        struct deduce_source_char
           {
          // We need this to reproduce the bug.
             typedef typename stream_char_common< T >::type stage1_type;
             typedef typename deduce_source_char_impl< stage1_type >::type stage2_type;
           };

        template <class Source, class Target>
        struct lexical_cast_stream_traits 
           {
          // We need this to reproduce the bug.
             typedef typename remove_cv<Source>::type no_cv_src;

          // We need this to reproduce the bug.
             typedef deduce_source_char<no_cv_src> deduce_src_char_metafunc;

          // This statement is required to reproduce the bug.
             typedef typename deduce_src_char_metafunc::stage1_type is_source_input_not_optimized_t;
           };
      }
}

// std::string value;
// bool value;
// std::size_t value;

void foobar() 
   {
  // boost::detail::lexical_cast_stream_traits<std::size_t, std::string>::deduce_src_char_metafunc::stage1_type is_source_input_not_optimized_t = value;
  // boost::detail::lexical_cast_stream_traits<std::size_t, std::string>::deduce_src_char_metafunc::stage1_type is_source_input_not_optimized_t = 42;
  // boost::detail::lexical_cast_stream_traits<std::size_t, std::string>::deduce_src_char_metafunc::stage1_type is_source_input_not_optimized_t = value;
     boost::detail::lexical_cast_stream_traits<std::size_t, std::string>::deduce_src_char_metafunc::stage1_type is_source_input_not_optimized_t;
   }
