// t0486.cc
// overloaded member function templates, determination of
// which to use is done via specificity partial order, where
// we have to compare U to B<T> *twice* (once in return type,
// once in parameter list)

template <class T>
struct B {};

struct A {
  template <class U>
  U f(U);

  template <class T>
  B<T> f(B<T>);
};

void g(A& a, B<int>& b) {
  a.f(b);
}


// ---------------------
namespace Blah {
  template <class T>
  struct B {};

  struct A {
    template <class U>
    U f(U);

    // true ambiguity
    //ERROR(1): template <class T>
    //ERROR(1): B<T*> f(B<T>);
  };

  void g(A& a, B<int>& b) {
    a.f(b);
  }
}


// --------------------
namespace Whatever {
  template <class U, class W>
  U f(W, U);

  // NOTE: Currently this works but for the wrong reason!
  // t0487.cc shows the problem.  Once t0487.cc is fixed then
  // this should *stop* working, because I have not implemented
  // structural equality in DependentQType::innerEquals.
  //
  // 2005-08-03: This now appears to be fixed, in part by using
  // the new mtype module.
  template <class T>
  typename T::INT f(T*, typename T::INT);
         
  struct C {
    typedef int INT;
  };

  void g(C *c, int i)
  {
    f(c, i);
  }
}

