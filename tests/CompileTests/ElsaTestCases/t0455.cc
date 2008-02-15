// t0455.cc
// PQName ambiguity


namespace N1 {
  template <class T>
  struct A {
    int x;
  };

  template <int n>
  struct B {
    template <int m>
    struct C {};
  };

  int const D = 1;

  void foo(int val)
  {
    // +--------------------------+
    //     +--------------------+
    //                +---------+
    //     +-----+        +---+
       A < B < 3 > :: C < D < 3 > >  a;
       
       a.x = val;
  }
}


namespace N2 {
  template <int n>
  struct A {
    template <class U>
    struct C {
      int x;
    };
  };

  int const B = 2;

  template <int m>
  struct D {};

  void foo(int val)
  {
    // +--------------------------+
    // +---------+    +-----------+
    //     +---+          +-----+
       A < B < 3 > :: C < D < 3 > >  c;

       c.x = val;
  }
}


// EOF
