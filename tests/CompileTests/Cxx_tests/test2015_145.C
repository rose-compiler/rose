class A
   {
#if 0
     public:
          int x;
#endif
   };

class B
   {
#if 1
     public:
          int x;
#endif
   };

class C : public A, public B
   {
     public:
#if 1
          int x;
#endif
   };

void foobar()
   {
     C m;
  // m.A::x = 7;
     m.B::x = 7;
   }
