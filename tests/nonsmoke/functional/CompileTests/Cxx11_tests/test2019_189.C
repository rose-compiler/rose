
namespace S_
   {
     struct B
        {
          template <int i> int foo();
        };

     struct D : B
        {
          int foo(int i);
        };
   }

void f_(S_::D *p)
   {
  // Original code: p->S_::B::f<2>();
  // Unparsed as: p -> f< 2 > ();
     p->S_::B::foo<2>();
   }


