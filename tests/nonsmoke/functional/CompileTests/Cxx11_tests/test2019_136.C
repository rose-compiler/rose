class Base1
	{
          public:
               int a;
	};

struct D : public Base1 { };
struct E : public Base1 { };

struct X : public D, public E { };

void foobar()
   {
     X x,y;
  // Unparsed as: x.a (missing name qualification: "D::").
  // Note that "Base1::" is still ambigous, and that "D::Base1:: is not allowed.
  // So "D::" is the only name qualification allowed to be used (and is the 
  // mimimal qualification required to resolve the ambiguity).
     x.D::a;
     y.D::a;
   }

