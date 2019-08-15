
class Base1
	{
          public:
               void bar();
	};

struct D : public Base1 { };
struct E : public Base1 { };

struct X : public D, public E { };

void foobar()
   {
     X x;
     x.D::bar();
     x.E::bar();
   }


