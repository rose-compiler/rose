// This is an attempt to isolate the bug in compiling
// ROSE/src/ROSETTA/src/grammar.C using ROSE (unfinished).
template <typename T>
class X
   {
     public:
          X();
   };

// namespace A
struct A
   {
     typedef int A_type;
   };

class Y
   {
     public:
          typedef X<A::A_type> X_type;
          Y(X_type x);
   };

void foobar(Y::X_type)
   {
  // Both of these are unparsed t: foobar(::X < int > ::X());
  // This can be important to do when Y::X_type is a private type.
     foobar(X<A::A_type>());
     foobar(Y::X_type());
   }
