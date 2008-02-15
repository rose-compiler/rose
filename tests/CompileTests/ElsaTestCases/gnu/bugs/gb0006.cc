// gb0006.cc
// another permissive issue

enum State { goodbit };

template <class T>
void f(State &e)
{
  e |= goodbit;
}
