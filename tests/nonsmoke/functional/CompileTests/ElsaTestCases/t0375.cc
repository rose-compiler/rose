// t0375.cc
// copy ctor cannot be a template

struct S {
  // this member template is not the copy constructor because
  // it is a template (12.8 para 2), and will not be used below
  template <class T>
  S(S const &s, T *t = 0)
  {
    int x = T::doesNotExist;
  }
};

void foo(S const &src)
{
  S s(src);      // calls compiler-supplied implicit copy ctor
}

