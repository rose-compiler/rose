
namespace mpl_ {
   template< bool C_ > struct bool_;
   typedef bool_<true> true_;
   typedef bool_<false> false_;
}

namespace boost { namespace mpl { using ::mpl_::bool_; } }
namespace boost { namespace mpl { using ::mpl_::true_; } }
namespace boost { namespace mpl { using ::mpl_::false_; } }


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


namespace boost { namespace mpl { namespace aux {

typedef char (&no_tag)[1];
typedef char (&yes_tag)[2];

template< bool C_ > struct yes_no_tag
   {
     typedef no_tag type;
   };

template<> struct yes_no_tag<true>
   {
     typedef yes_tag type;
   };

}}}


namespace boost { 
namespace mpl { 
namespace aux {
   template< typename T, typename fallback_ = boost::mpl::bool_<false> > 
   struct has_apply 
      {  
        struct gcc_3_2_wknd 
           { 
             template< typename U > static boost::mpl::aux::yes_tag test( boost::mpl::aux::type_wrapper<U> const volatile* , boost::mpl::aux::type_wrapper<typename U::apply> * = 0 ); 
             static boost::mpl::aux::no_tag test(...);
           }; 
        typedef boost::mpl::aux::type_wrapper<T> t_; static const bool value = sizeof(gcc_3_2_wknd::test(static_cast<t_* >(0))) == sizeof(boost::mpl::aux::yes_tag); typedef boost::mpl::bool_<value> type;
      };
}
}
}



namespace boost { 
namespace mpl {

template< typename F, typename has_apply_ = typename aux::has_apply<F>::type > struct apply_wrap0 : F::template apply<  > {};

template< typename F > struct apply_wrap0< F,true_ > : F::apply {};

template< typename F, typename T1  > struct apply_wrap1 : F::template apply<T1> {};
template< typename F, typename T1, typename T2 > struct apply_wrap2 : F::template apply< T1,T2 > {};
template< typename F, typename T1, typename T2, typename T3 > struct apply_wrap3 : F::template apply< T1,T2,T3 > {};
template< typename F, typename T1, typename T2, typename T3, typename T4 > struct apply_wrap4 : F::template apply< T1,T2,T3,T4 > {};
template< typename F, typename T1, typename T2, typename T3, typename T4, typename T5 > struct apply_wrap5 : F::template apply< T1,T2,T3,T4,T5 > {};
}}


template<class F>
class vector_unary
   {
     public:
          void foobar (int i) const 
            {
           // We don't really know what this is when we translate it (it shows up as a class).
              F::apply ();
            }
   };



