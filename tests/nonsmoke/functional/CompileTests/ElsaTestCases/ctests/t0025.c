// t0025.c
// pointer and null ptr constant args to ?:

typedef struct A {
  int x;
} A;

void foo(int c, A *a)
{
  (c? 0 : a)->x;
  (c? a : 0)->x;

  (c? (void*)0 : a)->x;
  (c? a : (void*)0)->x;
}
