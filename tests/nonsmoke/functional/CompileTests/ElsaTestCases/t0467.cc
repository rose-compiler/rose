// t0467.cc
// another from A.E.

// The issue with this test is that the call to 'f' below is
// syntactically ambiguous, possibly using one or two arguments, but
// the proper # of args is needed to do overload resolution.

template <class T1, class T2>
struct C {
  C(const T1&, const T2&);
};

void f(C<int,int> const &);
void f(int const &);

typedef int INT;

void g()
{
  f(C<INT,INT>(0,0));
}
