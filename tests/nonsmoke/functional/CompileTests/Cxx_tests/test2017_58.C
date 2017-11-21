
#if 0
template <typename Ptr, typename U>
struct pointer_has_rebind
{
   template <typename X>
   static char test(int, typename X::template rebind<U>*);
};
#endif

template <typename U>
struct pointer_has_rebind
{
   template <typename X>
   char test(typename X::template rebind<U>*);
};
