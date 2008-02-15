// t0487.cc
// matching against a DQT (see also t0488.cc)

template <class T>
void f(T*, typename T::INT, int);

struct C {
  typedef int INT;
};

void g(C *c, int i)
{
  f(c, i, 1);
}
