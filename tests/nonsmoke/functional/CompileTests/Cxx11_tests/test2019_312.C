// The volatile function modified for non-member functiosn is not unparsed.

class X_;

int (X_::*pf_)();
int (X_::*pcf_)() const;

// BUG: Unparsed as: int (::X_::*pvf_)();
int (X_::*pvf_)() volatile;

// BUG: Unparsed as: int (::X_::*pcvf_)() const ;
int (X_::*pcvf_)() const volatile;

class X_
   {
public:
	int f() { return 1; }
	int fc() const { return 2; }
	int fv() volatile { return 3; }
	int fcv() const volatile { return 4; }
	X_() { }
	X_(int ii) { }
   };

void foobar()
   {
     X_ x;
     pf_ = &X_::f;

     const X_ cx (0);
     pcf_ = &X_::fc;

     volatile X_ vx;

  // Unparsed as:  pvf_ = &X_::fv;
     pvf_ = &X_::fv;

     const volatile X_ cvx (0);

  // Unparsed as: pcvf_ = &X_::fcv;
     pcvf_ = &X_::fcv;
   }

