// t0238.cc
// same as t0234/t0237, except defn not provided

// fwd
template <class S1, class T1>
int foo(S1 *s, T1 *t);

// instantiation
void f()
{
  // use explicit template argument syntax
  int *s;
  float *t;
  foo<int,float>(s, t);

  // let the template arguments be deduced from the function arguments
  char *s2;
  double *t2;
  foo(s2, t2);
}

// no defn


// EOF
