template<class T> class A { int x; };
template<template<class T> class V > class B {};

// Declaration using template type taking template template argument
B<A> c;

