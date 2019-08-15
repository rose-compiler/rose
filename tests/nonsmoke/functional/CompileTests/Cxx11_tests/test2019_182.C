
template <class T> class X_ { };
template <class T> void f_(T t) { }
struct { } a_;
void f_() {
	struct A { };
	enum { e1 };
	typedef struct {} B;
	B b;
	X_<A> x1;
	X_<A*> x2;
	X_<B> x3;
	f_(e1);
	f_(a_);
	f_(b);
}

