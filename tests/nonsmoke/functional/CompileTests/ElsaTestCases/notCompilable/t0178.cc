// t0178.cc
// refer to a member template using "template" keyword in qualified name

struct A {
  template <class T>
  struct B {
    static int foo();
    typedef int other;
  };
};

int main()
{
  typedef A::template B<int>::other myother;
  return A::template B<int>::foo();
}

