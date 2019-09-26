
class Base1
   {
     public:
          Base1();
          int foo();
#if 1
          int foo(int i);
#endif
   };

struct D : public Base1 { };
struct E : public Base1
   {
     E() : Base1() { }
   };

struct X : public D, public E { };

void foobar()
   {
     X x;
     x.D::foo();
   }


