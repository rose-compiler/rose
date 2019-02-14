
class Base1
	{
public:
	int a, b;
	Base1(int i = 1, int j = 2) : a(i), b(j) { }
	int foo() { return a + b; }
	int foo(int i) { return a + b + i; }
	};

struct Base2
	{
	int c, d;
	Base2(int i = 3, int j = 4) : c(i), d(j) { }
	int bar() { return c + d; }
	class Base3
		{
public:
		int e, f;
		Base3(int i = 5, int j = 6) : e(i), f(j) { }
		};
	};

struct ABC
	{
	int i;
	ABC(int ii) : i(ii) { }
	virtual int f(int) = 0;
	virtual int g(int) = 0;
	virtual int operator *(int) = 0;
	};


struct D : public Base1, public Base2 { };
struct E : public Base1, public Base2
   {
     E() : Base1(11, 12), Base2(13, 14) { }
   };

struct X : public D, public E { };

void foobar()
   {
     X x;
     x.D::a;
     ((D &)x).Base2::d;
     x.E::b;
     ((E &)x).Base2::c;
     x.D::foo();
     x.E::foo();
   }


