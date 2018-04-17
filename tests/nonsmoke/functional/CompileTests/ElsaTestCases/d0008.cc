// d0008.cc
// tricky sequence of conversions, involving inheriting from template param

// template inheriting from its parameter
template <class T>
struct D : T {};

// mostly irrelevant
struct Q {};

// template with conversion operator to D<T>*
template <class T>
struct C {
  C (Q &);
  operator D<T> *();
};

struct A {};
struct B {};

// overloaded 'g'
int g(A*);
int g(B*);

void f()
{
  Q q;
  C<B> r(q);

  // Force an instantiation of D<B>; does this help?
  //
  // Yes, this makes the conversion available.  Now to find
  // out how to make it available w/o a forced instantiation...
  //
  // Got it, 14.7.1 para 4 says what to do, and it's implemented
  // in overload.cc, look for 'ensureClassBodyInstantiated'.
  //D<B> dummy;

  // expected sequence of conversions:
  //   'r' is of type C<B>, so can convert to D<B>* (user defined conv)
  //   D<B> inherits from B, so D<B> can convert to B* (derived to base)
  //   B* is one of the allowed params of 'g'
  g(r);
}
