// t0250.cc
// explicit instantiation of a template class

template <class T>
class A {
  int foo() { return 2; }
  int bar();
  int baz();
};

// out-of-line defn before inst req
template <class T>
int A<T>::bar()
{
  return 3;
}

// explicit instantiation request
template class A<int>;

// out-of-line defn after inst req
template <class T>
int A<T>::baz()
{
  return 4;
}
