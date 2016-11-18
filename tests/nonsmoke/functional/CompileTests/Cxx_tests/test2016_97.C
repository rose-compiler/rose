
template<template<class...> class F, class... T>
struct mp_valid_impl
{
    template<template<class...> class>
    static void check(...);
};

