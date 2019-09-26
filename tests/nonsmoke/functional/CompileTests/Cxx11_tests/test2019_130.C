
class Base1
	{
          public:
               int a;
	};

struct D : public Base1 { int fff; };
struct E : public Base1 { int ggg; };

struct X : public D, public E { };

void foobar()
   {
     X x;
  // Unparsed as: x.a (missing name qualification).
     x.D::a;
   }


