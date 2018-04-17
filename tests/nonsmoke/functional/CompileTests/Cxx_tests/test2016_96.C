namespace boost
{

namespace type_traits_detail
{

// mp_valid
// implementation by Bruno Dutra (by the name is_evaluable)

template<template<class...> class F, class... T>
struct mp_valid_impl
{
#if 0
    template<template<class...> class G, class = G<T...>>
 // static boost::true_type check(int);
    static void check(int);
#endif

#if 1
    template<template<class...> class>
 // static boost::false_type check(...);
    static void check(...);
#endif

 // using type = decltype(check<F>(0));
};

#if 0
template<template<class...> class F, class... T>
using mp_valid = typename mp_valid_impl<F, T...>::type;

// mp_defer

struct mp_empty
{
};

template<template<class...> class F, class... T> struct mp_defer_impl
{
    using type = F<T...>;
};

// template<template<class...> class F, class... T> using mp_defer = typename boost::conditional<mp_valid<F, T...>::value, mp_defer_impl<F, T...>, mp_empty>::type;
#endif

} // namespace type_traits_detail

} // namespace boost
