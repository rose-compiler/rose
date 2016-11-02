// t0509.cc
// template accepts int&, argument is int const

// Elsa, edg and gcc all reject this input, I think for the same
// reason: they see the 'int const' and decide the argument is '4',
// when in fact it needs to be (reference to) 'x'.  But if it
// doesn't work in those systems, I won't bother fixing it in
// Elsa either (for now).

// Nevertheless, I believe the code is valid C++.

template <int &IR>
int f()
{
  &IR;       // legal
  return IR + 10;
}

extern int const x = 4;

void foo()
{
  f<x>();
}
