#include <stdbool.h>

int a;
float b;
double c;
long double d;
struct A {
  long v;
  int x;
  short y;
  bool z;
  double w;
};
struct A e;
union B {
  int x;
  short y;
  double z;
  struct A w;
};
union B f;
struct C {
  int x;
  short y;
  double z;
  char zz;
  struct A w;
  short v[2];
};
struct C g;
typedef union {
  short y[2];
  long z;
} U;
struct D {
  int x;
  union {
    short y[2];
    int z;
  };
  U v[5];
  short u[5];
  char w[7];
  int t;
};
struct D h;
