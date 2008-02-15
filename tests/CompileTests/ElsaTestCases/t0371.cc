// t0371.cc
// arg-dep lookup of template-id

namespace N {
  class A {};
  
  template <class T>
  void foo(A &a, T t);
};

template <class T>
void foo(int x, int y, int z);

void bar(N::A &a)
{                
  // the "<int>" following "foo" is permitted only because lookup of
  // "foo" yields (at least?) a template name; but then, since the
  // name is unqualified, arg-dep lookup augments the original lookup
  // set with N::foo, which is the one chosen (14.8.1 para 6)
  //
  // interestingly, gcc does not like this syntax, though icc does
  foo<int>(a, 3);
}
