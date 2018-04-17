// t0448.cc
// nondependent lookup..

template <class T>
struct B {
  int foo(int);
};

template <class T>
struct D : B<T> {
  int bar()
  {
    //ERROR(1): return foo(3);          // nondependent--illegal
  }

  int baz()
  {
    return this->foo(3);    // ok
  }
};

template <class T>
struct E : B<int> {
  int blorf()
  {
    return foo(4);          // legal!
  }

  int garf()
  {
    //ERROR(2): return nonexist(5);     // non-dependent but no unsearched bases
  }
};

template <class T>
struct F : T::Q {
  int f()
  {
    //ERROR(3): return foo(5);
  }
};

