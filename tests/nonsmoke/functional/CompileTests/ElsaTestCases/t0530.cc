// t0530.cc
// confusion between operator[] and operator*

struct A {
  operator float *();
  
  // Elsa was getting messed up by the 'const' on 'i', which should
  // not participate in overload resolution
  float operator [] (const int i) const;
};

void foo()
{
  A a;
  a[0];   // operator[] is the right choice
}


// problem with recursive tcheck
struct B {
  B (const float * xyz);

  operator float *();

  friend B operator + (const B & v1, const B & v2);
};

void foo(B &b)
{
  b[0];     // translated to *(a+0), but with '+' as built-in
}
