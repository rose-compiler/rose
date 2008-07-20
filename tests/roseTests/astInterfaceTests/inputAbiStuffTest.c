#include <stdbool.h>

int a;
float b;
double c;
long double d;
struct A {
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
};
struct C g;
