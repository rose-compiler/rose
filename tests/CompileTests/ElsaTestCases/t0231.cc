// t0231.cc
// func template defn and inst within a namespace

namespace N {
  template <class T>
  int foo(T *t)
  {
    return 2;
  }
  
  void g()
  {
    int *x;
    foo(x);
  }
}

