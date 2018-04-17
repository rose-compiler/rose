template <typename T> struct cv_traits_imp {};

// const bool value = cv_traits_imp<T * > ::is_volatile;

#if 1
template <class T>
struct is_volatile_rval_filter
   {
  // static const bool value = ::boost::detail::cv_traits_imp<T * > ::is_volatile;
     static const bool value = cv_traits_imp<T * > ::is_volatile;
   };
#endif

#if 1
template <class T>
struct rvalue_ref_filter_rem_cv
   {
  // typedef typename boost::detail::cv_traits_imp<T * >::unqualified_type type;
  // typedef typename cv_traits_imp<T * >::unqualified_type type;
     static const bool value = cv_traits_imp<T * > ::is_volatile;
   };
#endif

#if 1
template <class T>
struct is_const_rvalue_filter
   {
  // static const bool value = ::boost::detail::cv_traits_imp<T * > ::is_const;
  // static const bool value = cv_traits_imp<T * > ::is_const;
     static const bool value = cv_traits_imp<T * > ::is_volatile;
   };
#endif
