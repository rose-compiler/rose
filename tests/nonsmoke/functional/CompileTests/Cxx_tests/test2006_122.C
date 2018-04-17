// Example showing use of template template arguments

#if 0
// This code is an example of a C++ error, not a bug in ROSE!
template<class T, int i> class A { int x; };
template<class T> class A<T, 5> { short x; };

// The error is that this template must be defined 
// to use the template template argument.
template<template<class T> class U> class B1 { };

// Declaration using template type taking template template argument
B1<A> c;
#endif


// We don't need the iostream header file if we are not using "typeid()"
// #include <iostream>
// using namespace std;

template<class T, class U> class A { int x; };
template<class U> class A<int, U> { short x; };

// This example defines the template to use the 
// template template argument and is valid C++ code.
template<template<class T, class U> class V> class B
   {
  // Use of template template argument to build instantated template types.
     V<int, char>  i;
     V<char, char> j;
   };

// Declaration using template type taking template template argument
B<A> c;

int main()
   {
  // DQ: This does not work in EDG, I don't know why!
  // cout << typeid(c.i.x).name() << endl;
  // cout << typeid(c.j.x).name() << endl;
   }
