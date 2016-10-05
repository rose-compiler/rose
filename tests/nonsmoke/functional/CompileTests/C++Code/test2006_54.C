// Class declarations galore.

class A
{
public:
  int foo (int x) { return x; }
  int bar (const char* s)
  {
    class A
    {
    public:
      int foo (int x) { return x; }
    } x;
    return x.foo (0);
  }

  class A1
  {
  public:
    int foo (int x) { return x; }
  };

protected:
  class A1_
  {
  public:
    int foo (int x) { return x; }
  };

private:
  class A1__
  {
  public:
    int foo (int x) { return x; }
  };
};

template <typename T>
class A_T
{
public:
  int foo (T x) { return (int)x; }
};

template class A_T<int>;
template class A_T<double>;
template class A_T<const void *>;

namespace B
{
  class B
  {
  public:
    int foo (int x) { return x; }
    int bar (const char* s)
    {
      class A
      {
      public:
        int foo (int x) { return x; }
      } x;
      return x.foo (0);
    }
    operator bool () { return false; }
  } b;
}

void foo (int x)
{
  class A
  {
  public:
    A () {}
    int foo (int x) { return x; }

    A (int) {}
    operator bool () { return false; }
  } x1;

  try
    {
      class A
      {
      public:
        int foo (int x) { if (!x) throw -1; return x; }
      } x2;
      x2.foo (0);
    }
  catch (const A& x)
    {
    }
  catch (int)
    {
      class A
      {
      public:
        int foo (int x) { throw x; }
      } x2;
    }

  do
    {
      class A
      {
      public:
        int foo (int x) { return x; }
      } x2;
    }
  while (x == 5);

  for (A_T<bool> x2; x2.foo (false);)
    for (class A0 {public: int foo (int x) {return x;}} x2; x2.foo (0); x)
      for (; A x2 = 0;)
        while (A x2 = 0)
          if (A x2 = 0)
            class A { public: int foo (int x) { return x; } } x3;
          else if (B::b)
            class A { public: int foo (int x) { return x; } } x3; 
          else
            class A { public: int foo (int x) { return x; } } x3;

  for (A_T<bool> x2; x2.foo (false);)
    for (class A0 {public: int foo (int x) {return x;}} x2; x2.foo (0); x)
      for (; A x2 = 0;)
        while (A x2 = 0)
          if (A x2 = 0)
            class A { public: int foo (int x) { return x; } } x3;
          else if (B::b)
            class A { public: int foo (int x) { return x; } } x3; 
          else
            class A { public: int foo (int x) { return x; } } x3;
}

// eof
