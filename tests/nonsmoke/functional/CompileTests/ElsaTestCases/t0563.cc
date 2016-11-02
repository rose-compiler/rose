// t0563.cc
// tricky template specificity test involving DQTs

struct A {
  typedef int INT;
};

template <class T>
void f(A *a, T i)
{
  // I know ICC and GCC choose this one because they report this line
  // as an error when uncommented.
  //typename T::NONEXIST blah;
}

template <class S>
void f(S *a, typename S::INT i)
{
  //typename S::NONEXIST2 blah2;
}

void foo(A *a, int i)
{             
  // I believe this call is ambiguous because both templates
  // match, yet neither can match with the other.  However, both
  // ICC and GCC choose the first 'f'.
  //ERROR(1): f(a,i);
}



template <class T>
void g(A *a, int i, T f);

template <class S>
void g(S *a, typename S::INT i, float f);

void goo(A *a, int i, float f)
{
  // I believe this one is ambiguous as well.  ICC and GCC agree.
  //ERROR(2): g(a,i,f);
}

