// t0137.cc
// overloading of operator[]
  
void dummy();               // line 4

struct A {
  int operator[] (int);     // line 7
};


void f()
{
  int *p;
  A a;

  // turn on overload resolution
  __testOverload(dummy(), 4);

  __testOverload(p[4], 0);      // built-in
  __testOverload(a[4], 7);      // overloaded
}


// violate some operator[] rules
//ERROR(1): void operator[](int x);     // not a member

struct B {
  //ERROR(2): static void operator[](int x);    // static
  //ERROR(3): void operator[]();                // too few params
  //ERROR(4): void operator[](int x, int y);    // too many params
};





