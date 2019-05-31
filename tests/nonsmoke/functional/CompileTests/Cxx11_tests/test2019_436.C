
class A {};

class B;

class C
   {
     public:
          C (B x);
          A foo(void) const;
   };



class D: public C
   {
     public:
          D(const C & x);
          A foo(void);
   };

class B
   {
     public:
          B();
          A foo();
   };

void foobar(B x)
   {
     D y = C(x);
  // ROSE will unparse as: "D y(C(x));" which is confused with a function syntax for a function y.
  // Then calling foo from function y is not defined.
     y.foo();
   }

