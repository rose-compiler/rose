namespace Y
   {
#if 0
     typedef struct type_B typedefType_B;
#else
     struct type_B *Bptr;
#endif
   }

template < typename T > class A {};

// This does not unparse properly ("A<type_B> N;", lacks qualified name for type "type_B")
A<Y::type_B> N;
