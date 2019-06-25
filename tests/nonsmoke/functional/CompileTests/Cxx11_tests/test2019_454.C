class A {};

class B;

class C
   {
     public:
          C (B x);
          A foo(void);
   };



class D: public C
   {
     public:
          D(const C & x);
       // D (C & x);
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
  // Note: variables need to store information about how they were initialized so that we 
  // can generate the correct initialization syntax.  Though I thought that they were 
  // symantically equivalent.

  // Original code: D y = C(x);
  // Bug: class D y(C(x));
     D y = C(x);
     y.foo();
   }

