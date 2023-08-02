template<template <typename> typename T>
class A
   {
   };

namespace B
   {
     template <typename T> class C {};
     typedef A<C> typeZ;
   }

namespace E
   {
     template <typename T> class C {};
     typedef A<C> typeZ;
   }

// Example of template template argument.
typedef A<B::C> typeG;
typedef A<E::C> typeH;
