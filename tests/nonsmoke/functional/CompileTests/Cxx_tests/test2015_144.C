class A
   {
     public:
          int x;
   };

class B
   {
#if 0
     public:
          int x;
#endif
   };

class C : public A, B
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
     m.A::x = 7;
   }
