#if 0
struct D_ : public Base1, public Base2 { };
struct E_ : public Base1, public Base2
   {
     E_() : Base1(11, 12), Base2(13, 14) { }
   };

struct X_ : public D_, public E_ { };

void foobar()
   {
     X_ x;
     x.D_::a;
     (D_ &)x).Base2::d;
     x.E_::b;
     (E_ &)x).Base2::c
     x.D_::foo();
     x.E_::foo();
   }
#endif


