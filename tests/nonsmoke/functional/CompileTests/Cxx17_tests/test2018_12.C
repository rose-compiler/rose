// Allow typename in a template template parameter

template<typename T> struct A {};
template<typename T> using B = int;

template<template<typename> class X> struct C {};
C<A> ca; // ok

// This appears to compile fine with c++11 under gnu g++ 6.1 (so maybe it is not really C++17 specific).
C<B> cb; // ok, not a class template

// DQ (1/1/2019): I think that this should fail, but it compiles fine with EDG 5.0 and gnu g++ version 6.1.
template<template<typename> typename X> struct D; // error, cannot use typename here
