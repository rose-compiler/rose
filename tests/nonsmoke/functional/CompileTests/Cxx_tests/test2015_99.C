


#if 0
template < typename Lhs, typename Rhs, typename Ret >
struct trait_impl {
   typedef typename ::boost::remove_reference<Lhs>::type Lhs_noref;
   typedef typename ::boost::remove_reference<Rhs>::type Rhs_noref;
   typedef typename ::boost::remove_cv<Lhs_noref>::type Lhs_nocv;
   typedef typename ::boost::remove_cv<Rhs_noref>::type Rhs_nocv;
   typedef typename ::boost::remove_cv< typename ::boost::remove_reference< typename ::boost::remove_pointer<Lhs_noref>::type >::type >::type Lhs_noptr;
   typedef typename ::boost::remove_cv< typename ::boost::remove_reference< typename ::boost::remove_pointer<Rhs_noref>::type >::type >::type Rhs_noptr;
#if 1
  // This statement is required to demonstrate the bug.
   static const bool value = (trait_impl1 < Lhs_noref, Rhs_noref, Ret, ::boost::type_traits::ice_or< ::boost::type_traits::ice_and< ::boost::is_fundamental< Lhs_nocv > ::value, ::boost::is_fundamental< Rhs_nocv > ::value, ::boost::type_traits::ice_or< ::boost::type_traits::ice_not< ::boost::is_integral< Lhs_noref > ::value > ::value, ::boost::type_traits::ice_not< ::boost::is_integral< Rhs_noref > ::value > ::value > ::value > ::value, ::boost::type_traits::ice_and< ::boost::is_fundamental< Lhs_nocv > ::value, ::boost::is_pointer< Rhs_noref > ::value > ::value, ::boost::type_traits::ice_and< ::boost::is_fundamental< Rhs_nocv > ::value, ::boost::is_pointer< Lhs_noref > ::value > ::value, ::boost::type_traits::ice_and< ::boost::is_pointer< Lhs_noref > ::value, ::boost::is_pointer< Rhs_noref > ::value > ::value > ::value > ::value);
#else
  static const bool value = false;
#endif
};

} 
} 

#if 1
  // This statement is required to demonstrate the bug.
    template< typename Lhs, typename Rhs=Lhs, typename Ret= ::boost::detail::has_left_shift_impl::dont_care > struct has_left_shift : public ::boost::integral_constant<bool,(::boost::detail::has_left_shift_impl::trait_impl < Lhs, Rhs, Ret > ::value)> { public:   }; 
#else
    template< typename Lhs, typename Rhs=Lhs > struct has_left_shift { public: static const bool value = false;  }; 
#endif
}

#endif


// namespace has_left_shift_impl {

template < typename T >
struct Y 
   {
     typedef T type;
   };

// template < typename X > struct trait_impl1;

template < typename X >
struct trait_impl 
   {
     typedef typename Y<X>::type X_nocv;

  // static const bool value = (trait_impl1 < X >::value);
  // static const X_nocv value = (trait_impl1 < X >::value);
     static const bool value = false;
};

// }

template <class T, T val>
struct integral_constant // : public mpl::integral_c<T, val>
   {
     typedef integral_constant<T,val> type;
   };


// template <typename X > struct has_left_shift : public integral_constant<bool,(has_left_shift_impl::trait_impl<X>::value)> {};
template <typename X > struct has_left_shift : public integral_constant<bool,(trait_impl<X>::value)> {};

has_left_shift<bool> ABC;
