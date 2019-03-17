
double a_ = 1.0;

struct A 
   {
     int i;
   };

// BUG: unparsed as: struct B {} d;
struct B : A 
   {
     int j;
   } d_;

B b_() { return d_; }

const A& r_ = b_();

A&& s_ = b_(); // _RVALUE_REF

struct C 
   {
     operator B() { return d_; }
     operator double&() { return a_; }
     int j;
   } e_;


void foobar()
   {
	const A& a = e_;
	int b = 2;
	int&& c = static_cast<int&&>(b); // _RVALUE_REF
	B&& d = e_;
	a.i;
	c;
	d.j;
   }


