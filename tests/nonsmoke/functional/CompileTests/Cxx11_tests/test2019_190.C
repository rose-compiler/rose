
namespace S
   {
     struct B
        {
          void foo();
        };

     struct D : B
        {
          void foo(int i);
        };
   }

void foobar(S::D *p)
   {
  // p->S::B::foo();
     p->B::foo();
   }


