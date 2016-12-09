// t0351.cc
// pass 'Foo const' as a template argument

template <class T>
struct A {
  void foo(T *t);
};

struct B {};

void bar(B *);

void f()
{
  // first, make a distracting instantiation *without* the const;
  // this causes Elsa to re-use the distractor instantiation, thus
  // forgetting about the 'const'
  A<B> distractor;

  // now instantiate with 'const'
  A<B const> a;
  B const *p;

  // have to get 'Bar const *' as parameter type, which means not
  // ignoring the 'const' in the template argument
  a.foo(p);
  
  // not allowed
  //ERROR(1): bar(p);
}

// EOF
