// 7.3.1.2b.cc

namespace Q {
  namespace V {
    void f();
  }
  void V::f() { /*...*/ }                 // OK
  //ERROR(1): void V::g() { /*...*/ }     // error: g() is not yet a member of V
  namespace V {
    void g();
  }
}

namespace R {
  //ERROR(2): void Q::V::g() { /*...*/ }  // error: R doesn't enclose Q
}
