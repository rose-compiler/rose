// t0234.cc
// forward-declared function template with differing template
// parameter lists
                             
// fwd
template <class S1, class T1>
int foo(S1 *s, T1 *t);

// defn
template <class S2, class T2>
int foo(S2 *s, T2 *t)
{
  return sizeof(S2) + sizeof(T2);
}

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


// EOF
