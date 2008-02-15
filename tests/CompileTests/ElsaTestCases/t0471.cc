// t0471.cc
// arg-dep lookup does not find class members

struct A {
  static int f(A *a);
};


void foo()
{
  A *a = 0;

  //ERROR(1): f(a);

  // the above is not legal; 3.4.2p1 is not very clear, but the
  // apparent intepretation is that namespace members and
  // namespace-scope friends declared in classes (but not other class
  // members) are the only things found by arg-dep; some other
  // references:
  //
  //   http://groups-beta.google.com/group/comp.std.c++/browse_thread/thread/984bb13bc4ef5f7/a864e5af184ae877
  //   http://groups-beta.google.com/group/comp.lang.c++.moderated/browse_thread/thread/3456d46180a4e6c3/a08b673397148825
  //   http://www.open-std.org/jtc1/sc22/WG21/docs/cwg_active.html#218
  //   http://groups-beta.google.com/group/comp.std.c++/msg/e2cf2d0f8ac46fe9?hl=en
}


// EOF
