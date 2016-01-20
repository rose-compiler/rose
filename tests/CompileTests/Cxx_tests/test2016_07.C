template<class T, class U> class A { T t; };
// template<class U> class A<int, U> { U u; };

template<template<class T, class U> class V> class B { V<A<int,float>,float> v; };

// Declaration using template type taking template template argument
B<A> c;
