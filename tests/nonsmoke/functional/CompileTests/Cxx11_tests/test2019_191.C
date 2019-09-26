
struct B
   {
     void foo();
   };

struct D : B
   {
     void foo(int i);
   };

void foobar(D *p)
   {
     p->B::foo();
   }


