template <typename V>
class test
   {
     V v;
   };

#include <set>

template<typename V>
void foo()
   {
 // "typename" is required here...
     std::set<test<V> > s;
     typename std::set<test<V> >::iterator i;

     if (typename std::set<test<V> >::iterator i2=s.begin()) {}

  // These are not legal C++ code.
  // if ( (int x = 0) != 2) {}
  // if ( (typename std::set<test<V> >::iterator i3=s.begin() ) != s.end()) {}
   }

#if 1
template<template<class> class T, class V>
void foobar()
   {
     std::set<T<V> > XXX;
     typename std::set<T<V> >::iterator i = XXX.begin();
   }
#endif
