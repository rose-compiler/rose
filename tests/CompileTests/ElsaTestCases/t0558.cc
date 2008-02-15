// t0558.cc
// inline defn of friend in template class

template <class T>
struct A {
  friend int f(T t)
  {
    return sizeof(t);
  }

  T t;
  int g()
  {
    return f(t);
  }
};

void foo()
{
  A<int> a;
  a.g();
}
