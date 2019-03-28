namespace S_
   {
     const int N = 10;
   }

int a_[S_::N] = { 0 };

namespace S_
   {
     struct B
        {
          B(int i) : n(i) { }
          int n;
          template <int i> int *f() { return a_ + n * i; } // member template
        };

     struct D : B
        {
          D(int i) : B(i) { }
          int *f(int i) { return 0; }
        };

     enum color { red, white, blue };
   }

void f_(S_::D *p)
   {
  // unparsed as: p -> f< 2 > ();
     p->S_::B::f<2>();

  // unparsed as: &a_[p -> n * 2];
     &a_[p->S_::D::n * 2];
   }


void foobar()
   {
     S_::D d(S_::blue - S_::red + 1);
     f_(&d);
   }


