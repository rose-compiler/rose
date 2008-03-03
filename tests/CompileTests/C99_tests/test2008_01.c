// #include <stdio.h>

// Test of designators in structs
struct T {
  int w;
  char x;
};

struct S {
  int a[5];
  double b;
  struct T c;
};

int main(int argc, char** argv) {
  struct S x = {
    .b = 3.14,
#if 0 // EDG doesn't produce the correct AST for these (not a ROSE issue)
    .c.w = 8,
    .c.x = 7,
#endif
    .c = {.x = 7, .w = 8},
    a: { // An obsolete GCC syntax
      [1] = 1,
      [3] = 2,
      3
    },
  };
#if 0
  fprintf(stderr, "x = {\n");
  fprintf(stderr, "  a[0]=%d a[1]=%d a[2]=%d a[3]=%d a[4]=%d\n", x.a[0], x.a[1], x.a[2], x.a[3], x.a[4]);
  fprintf(stderr, "  b=%lf\n", x.b);
  fprintf(stderr, "  c = {\n");
  fprintf(stderr, "    w=%d\n", x.c.w);
  fprintf(stderr, "    x=%d\n", x.c.x);
  fprintf(stderr, "  }\n");
  fprintf(stderr, "}\n");
#endif
  return x.b == 3.14 && x.c.w == 8 && x.c.x == 7 && x.a[1] == 1 && x.a[3] == 2 && x.a[4] == 3 ? 0 : 1;
}
