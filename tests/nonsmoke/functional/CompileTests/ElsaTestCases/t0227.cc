// t0227.cc
// some experiments with the "partial scope stack"

namespace N {
  // eat templ inst scope
  template <class T>
  struct C {
    int f() {
      return 2;
    }

    inline int g();
  };

  // has DF_INLINE but the definition is actually out of line  
  template <class T>
  int C<T>::g()
  {
    return 3;
  }
}

namespace M {
  void func()
  {
    N::C<int> c;
    c.f();
    c.g();
  }
}

