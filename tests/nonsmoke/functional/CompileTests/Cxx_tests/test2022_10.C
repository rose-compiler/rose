template <int T>
class A
   {
   };

// Use two different expressions as a basis for building a template instantiation.
// These will be different types.
typedef A<1> typeA;
typedef A<42> typeB;

