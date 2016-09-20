// t0244.cc
// use 'this' in an inline method of a template class
// from nsAtomTable.i, nsSharedBufferHandle<CharT>::AcquireReference

template <class T>
class A {
  void func()
  {
    this;
  }
};

void f()
{
  A<int> *a;
  a->func();
}

