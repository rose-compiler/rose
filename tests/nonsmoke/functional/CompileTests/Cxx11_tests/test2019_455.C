class C
   {
     public:
          C (int x);
   };

class D: public C
   {
     public:
          D (const C & x);
       // int foo(void);
   };

void foobar()
   {
  // Original code: D y = C(x);
  // Bug: class D y(C(x));
     D y = C(42);


     D z(C(43));

  // This causes an error when it is unparsed as: class D y(C(x));
  // y.foo();
  // z.foo();
   }

