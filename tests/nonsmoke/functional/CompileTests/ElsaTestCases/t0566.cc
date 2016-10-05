// t0566.cc
// templatized conversion operator

template <class T>
struct B {};


struct A {
  template <class T>
  operator B<T> ()
  {
    // make sure this actually gets instantiated as a consequence
    // of being selected to do the conversion
    //ERROR(1): typename T::INT i;
  }

  // easy if the operator is not templatized
  //operator B<int> ();
};


void f(B<int> b);


void foo(A &a)
{
  f(a);
}
