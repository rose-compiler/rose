// t0026.cc
// template functions

template <class T>
int f(T t)
{
  T(z);    // ambiguous, but I can disambiguate!

  int q = T(z);    // ctor call

  // this error is correct:
  // non-compound `int' doesn't have fields to access
//    int y = t.x;
}

template <class T>
void g(T t, int);

int main()
{
  f(9);

  f<int>(8);

  //ERROR(1): f(1,2);        // too many arguments
  //ERROR(2): f();           // not all template arguments bound
  //ERROR(3): g(1);          // too few arguments (but all template args bound)

  // I get the wrong answer for this one..
  //f<int()>(8);
}
