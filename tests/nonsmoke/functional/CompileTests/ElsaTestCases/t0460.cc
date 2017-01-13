// t0460.cc

struct S {
  int x;
  int (*funcptr)(int);
  
  template <int n>
  int templfunc(int);
} *s;

int foo() {
  //ERROR(1): (s->funcptr<1>)(2);     // applying template args to non-template

  int (*anotherFuncPtr)(int);

  s->funcptr<anotherFuncPtr>(2);      // "<" and ">" parsed as less-then and greater-than

  s->x<1>(2);
  s->x < 1 || 2 > (3);
  
  s->templfunc<1>(2);
  (s->templfunc<1>)(2);
}

