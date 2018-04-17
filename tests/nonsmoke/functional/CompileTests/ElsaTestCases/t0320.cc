// t0320.cc
// parameter 'char' and argument 'char const &' are compatible

template <class T>
void f(T *t, char c);

void foo(char const &c)
{
  int *i;
  
  f(i, c);
}

// EOF
