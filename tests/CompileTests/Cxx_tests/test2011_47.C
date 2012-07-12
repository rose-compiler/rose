namespace Y
   {
  // This builds a type "type_B" and also a typedef type "typedefType_B".
     typedef struct type_B typedefType_B;
   }

// This is the templated class...
template < typename T > class A {};

// This is is a variable declaration of the templated class using a name qualified template argument.
// This does not unparse properly ("A<type_B> N;", lacks qualified name for type "type_B")
A<Y::type_B> N;
