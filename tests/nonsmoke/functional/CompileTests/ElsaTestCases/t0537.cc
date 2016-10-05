// t0537.cc
// float lits have type double, moron

// argument types will have to match
template <class T>
void a(T,T);

void foo()
{
  double d;
  a(d, 0.5 /*<-- my IQ*/);

  float f;
  a(f, 0.5f);
  
  long double ld;
  a(ld, 0.5l);
}
