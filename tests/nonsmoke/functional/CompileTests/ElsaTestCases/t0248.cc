// t0248.cc
// out-of-line definition of a member of a complete specialization

// primary
template <class T>
class A {
  int g();
};

// complete specialization
template <>
class A<int> {
  int f();
};

// out-of-line definition for specialization's f
// no "template <>" here!
int A<int>::f()
{
  return 2;
}

// out-of-line implicit specialization definition for primary's g
template <>
int A<float>::g()
{
  return 3;
}


