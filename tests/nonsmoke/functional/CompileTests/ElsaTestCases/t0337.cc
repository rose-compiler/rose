// t0337.cc
// situation requiring argument-dependent re-lookup

// some global function (not the one we want to call)
void f(char*);

template <class T>
struct A {
  // here is the one we want
  void f(T);

  // this is needed to make a complicated argument, below
  T otherFunc();
};

// but the one we want is only visible via a dependent base class
template <class T>
struct B : public A<T> {
  void foo()
  {
    T t;

    // the argument here is a little complicated; I think my
    // computation of what is dependent is being fooled by it
    f( this->otherFunc() );      // invoke 'f'
  }
};

void bar()
{
  B<int> b;
  b.foo();     // instantiate B<int>::foo
}



