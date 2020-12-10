class A {};

class B;

class C
   {
     public:
       // C (B x);
          C (const B & x);
       // C (int x);
          A foo(void) const;
   };



class D: public C
   {
     public:
          D(const C & x);
       // D(int x);
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
  // This must be unparsed using the assignment copy constructor syntax.
     D y = C(x);

  // This will be parsed as a function prototype, which cause the y.foo() expression to be wrong.
  // D y(C(x));

     y.foo();
   }

