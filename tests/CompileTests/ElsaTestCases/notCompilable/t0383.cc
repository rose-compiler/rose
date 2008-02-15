// t0383.cc
// from http://gcc.gnu.org/bugzilla/show_bug.cgi?id=11828

int x;
int y;

namespace n
{
  template <typename T>
  void
  f(T a)
  {
    a.f();
  }

  template <typename T>
  void
  g(T a)
  {
    x;
    n::f(a);
    y;
  }
}

struct S
{};

namespace n
{
  void f(S) {}       //ERRORIFMISSING(1): would find 'f' above
}

int
main()
{
  n::g(S());

  return 0;
}

//asm("collectLookupResults");
