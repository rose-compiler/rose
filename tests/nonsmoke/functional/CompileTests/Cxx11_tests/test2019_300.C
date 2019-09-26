struct B_
   {
	B_(int *pp) : p(pp) { }
	int *p;
	explicit operator void *() { return (void*)p; }
	operator int *() { return p; }
   };

struct D_ : B_
   {
	D_(int *p) : B_(p) { }
	void operator delete(void *q);
	static int delete_calls;
   };

int D_::delete_calls = 0;

void D_::operator delete(void *q)
   {
	++delete_calls;
	::operator delete(q);
   }


void foobar()
   {
  // delete_calls = 0;
     B_ b(new int(15));
  // ieq(*b, 15);
     delete b;
  // chk(delete_calls >= ivalue(1));
  // ::delete_calls = 0;
     D_::delete_calls = 0;
     D_ d(new int(15));
  // ieq(*d, 15);
     delete d;
  // ieq(D_::delete_calls, 0);
  // chk(delete_calls >= ivalue(1));
   }
