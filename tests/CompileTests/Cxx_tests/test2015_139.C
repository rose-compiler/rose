
class A
   {
     public:
          int x;
   };

class B
   {
     public:
          int x;
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
     m.A::x = 7;
   }
