
template <typename Ptr, typename U>
struct pointer_has_rebind
{
   template <typename X>
   static char test(int, typename X::template rebind<U>*);
};

