// t0018.c
// ambiguity involving attributed labels vs. implicit-int

void foo(int x, int y)
{
  label: __attribute__ ((__unused__))

  foo (x, y - 0 );
}
