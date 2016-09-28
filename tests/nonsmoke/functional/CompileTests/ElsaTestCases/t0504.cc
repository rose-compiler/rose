// t0504.cc
// partial specialization nested inside a template class

// This testcase motivated the introduction of several rather
// ugly hacks.  Maybe someday I will figure out the right way
// to implement this.

// container
template <class T>
struct A {
  // primary
  template <class U, class V>
  struct B {};

  // specialization
  template <class U>
  struct B <U, int> {
    typedef int INT;
  };
};

// instantiate
A<int> a;

A<int>::B<float,char> ab_prim;
A<int>::B<float,int> ab_spec;

// make sure we're getting the spec; the primary doesn't have INT
A<int>::B<float,int>::INT some_global_int;
