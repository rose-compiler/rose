// t0488.cc
// difficult type matching scenario

// 2005-08-15: Oops, my previous comments here were in error,
// the code was just wrong.  Elsa now accepts this.

template <class T>
void f(typename T::INT, T*, typename T::INT, int);

struct C {
  typedef int INT;
};

void g(C *c, int i)
{
  f(i, c, i, 1);
}
