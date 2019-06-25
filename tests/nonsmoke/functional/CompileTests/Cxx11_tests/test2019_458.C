
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
     D z(C(x));
  // y.foo();
   }

