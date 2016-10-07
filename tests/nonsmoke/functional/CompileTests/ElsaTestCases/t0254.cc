// t0254.cc
// dependent inner template scope and funky 'template' keyword

template <class S, class T>
struct A {
  typedef typename S::template Inner<T>::its_type my_type;

  // must use the 'template' keyword
  //ERROR(1): typedef typename S::Inner<T>::its_type another_my_type;
  
  // inner scope that is *not* a template
  typedef typename S::InnerNontemplate::another_type yet_another_type;

  // must not use 'template' if it is not a template
  //ERROR(2): typedef typename S::template InnerNontemplate::another_type yet_another_type2;
  
  
  // some tests where the name in question is at the end of the
  // qualifier chain, not in the middle
  typedef typename S::template Inner<T> inner_t;
  //ERROR(3): typedef typename S::Inner<T> inner_t2;
  
  typedef typename S::InnerNontemplate innerNontemplate;
  //ERROR(4): typedef typename S::template InnerNontemplate innerNontemplate2;
};

// one possible type to use in place of 'S'
struct Outer {
  template <class T>
  struct Inner {
    typedef T *its_type;
  };

  struct InnerNontemplate {
    typedef int another_type;
  };
};

// put it all together
int f()
{
  A<Outer, int>::my_type pointer_to_integer = 0;
  A<Outer, int>::yet_another_type integer = 0;

  int x = *pointer_to_integer;
  x = integer;
  
  return x;
}


// EOF
