#include <initializer_list>

int f_(std::initializer_list<int> a) { return (int)a.size(); }

typedef int IA_[3];

int h_(const IA_& b) { return b[0]; } 

struct A_ {
	int i;
	A_(std::initializer_list<double> a) : i(a.size()) { }
	A_(std::initializer_list<const char*> a) : i(a.size()) { }
};

int g_(A_ a) { return a.i; }

void foobar()
   {
     int a = f_( {1,2,3} );
  // ieq(a, 3);
     a = f_( {'a', 'b'} );
  // ieq(a, 2);
     a = g_( { "foo", "bar", "3", "4" } );
  // ieq(a, 4);
     a = h_( {1, 2, 3} );
  // ieq(a, 1);
   }

