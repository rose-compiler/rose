extern printf(const char *s, ...);

int a, * const b;

int foo(int a) {
  struct t {int a; const int b;};
//   struct t u = {7, 8};
//   struct t *w = &u;

/*  int x = a;
  int y = a > 0 ? a : -a;

  int z[5];

  z[4] = 5;

  while (x) {
    u.a = 24;
  }

  for(y = y+ 3; y != 0; y--) {
    w->a = u.a;
  }

  do {
    w->a--;
  } while (w->a);

  return printf("%d\n", y + 3[z] << u.a * w->b);*/
}