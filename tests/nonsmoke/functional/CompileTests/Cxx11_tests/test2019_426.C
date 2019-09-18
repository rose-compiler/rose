// This is a copy fo Cxx_tests/test2012_220.C, testing new name qualificaiton.
namespace Y
   {
     typedef struct type_B typedefType_B;
   }

template < typename T > class A {};

template < typename T = A < Y::type_B > > class I;
// template < typename T = Y::type_B > class I;

// Note that the templae instantiation "A<Y::type_B>" will be unique for each variable declared.
// This does not unparse properly ("A<type_B> N;", lacks qualified name for type "type_B")
A<Y::type_B> N;

#if 1
namespace Y
   {
     A<type_B> N2;
   }
#endif

