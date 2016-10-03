template <typename T> struct cv_traits_imp {};

#if 1
template <class T>
struct XXX
   {
     static const bool value = cv_traits_imp<T * > ::is_volatile;
   };
#endif

#if 1
template <class T>
struct YYY
   {
     static const bool value = cv_traits_imp<T * > ::is_volatile;
   };
#endif

