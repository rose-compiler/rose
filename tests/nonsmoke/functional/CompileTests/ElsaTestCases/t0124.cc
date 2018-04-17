// t0124.cc
// some more tests of pointer-to-member

class A {
public:
  int q(int);
  int qc(int) const;
  static int sq(int);
};


void f()
{
  int (A::*quint)(int);
  int (A::*quintc)(int) const;
  int (*squint)(int);

  quint = &A::q;
  //ERROR(1): quint = &(A::q);
  quintc = &A::qc;
  squint = &A::sq;
  squint = &(A::sq);   // ok
}
