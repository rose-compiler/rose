// t0205.cc
// instantiation leads to need for one name, while at the point
// of instantiation there is a hiding name

namespace N {
  typedef float A;            // name we want to find; it's a type

  template <class T>
  struct B {
    A x;                      // request for name "A" as a type in template body
  };

  struct C {
    int A;                    // hiding name; it's not a type
    B<int> y;                 // first instantiation
  };
}
