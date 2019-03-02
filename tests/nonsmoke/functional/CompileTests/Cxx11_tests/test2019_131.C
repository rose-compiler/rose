struct D { int a; };
struct E { int a; };

struct X : public D, public E { };

void foobar()
   {
     X x;
  // Unparsed as: x.a (missing name qualification).
     x.D::a;
   }


