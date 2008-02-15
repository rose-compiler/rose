// t0291.cc
// friend function with definition at point of declaration

struct A {
  friend int foo() { return 2; }
};


// 11.4 para 5 is (IMO) unclear about precisely what names are in
// scope in an inline friend definition; gcc likes the following
// code, so I'm putting it in my test suite too..

int I;                  // non-type; *not* seen

class B {
  typedef int I;        // type; this is what 'I' looks up to (at least in gcc)

  friend int bar()
  {
    I i;                // use of type 'I'
    return i;
  }
};
