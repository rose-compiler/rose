// t0548.cc
// member template class with out-of-line defn


namespace Case1 {
  template <class T>
  class A {
  public:
    template <class S>
    class B;
  };

  template <class T>
  template <class S>
  class A<T>::B {
  public:
    int x;
  };

  void foo()
  {
    A<int>::B<float> b;
    b.x;
  }
}

namespace Case2 {
  template <class T>
  class A {
  public:
    template <class S>
    class B;
  };

  // this intervening instantiation makes us create the declaration
  // of A<int>::B<S> before the definition of A<T>::B<S> is seen
  A<int> a;

  template <class T>
  template <class S>
  class A<T>::B {
  public:
    int x;
  };

  void foo()
  {
    A<int>::B<float> b;
    b.x;
  }
}


namespace Case3 {
  template <class T>
  class A {
  public:
    template <class S>
    class B;
            
    // mention B<T> before we'd get a chance to
    // transfer template member info
    B<T> *p;
  };

  template <class T>
  template <class S>
  class A<T>::B {
  public:
    int x;
  };

  void foo()
  {
    A<int>::B<int> b;
    b.x;
  }
}


// finally, the real case in the code I found

template <class T>
class A {
public:
  int front ();

  template <class S>
  class B;

  B<T> *d;
};

template <class T>
template <class S>
class A<T>::B {
public:
  int x;
  
  A<T> *p;
  B<S> *q;

  int f() { return 1; }
  int g();
  int h();
};

template <class T>
template <class S>
int A<T>::B<S>::g()
{
  return 2;
}

template <class T>
int A<T>::front()
{
  return d->x;
}

void foo()
{
  A<int> a;
  a.front();

  A<int>::B<float> b;
  b.x;
  b.f();
  b.g();
  b.h();
}

template <class T>
template <class S>
int A<T>::B<S>::h()
{
  return 3;
}

// force it to find the definition
template int A<int>::B<float>::h();
