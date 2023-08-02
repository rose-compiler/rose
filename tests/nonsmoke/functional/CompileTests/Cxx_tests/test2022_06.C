template <int T>
class A
   {
   };

// Use two different expressions as a basis for building a template instantiation.
// These will be the same type.
typedef A<1+2> typeA;
typedef A<1+1+1> typeB;
