// t0316.cc
// a 'T const &' param can be treated like 'T' ..

// why would you do this??
void f(int const &x);

void foo(unsigned int &y)
{                       
  f(y);
  
  int const &x = y;
}

