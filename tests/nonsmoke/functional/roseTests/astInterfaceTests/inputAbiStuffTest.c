#include <stdbool.h>
enum MyEnumType { ALPHA, BETA, GAMMA };
enum MyEnumType myenum1;

int array2[5][10][7];

typedef union u_SU
{
 short x;
 char a;
}SU;

SU myunion1;

_Bool mybool;
int a;
float b;
double c;
long double d;

double _Complex cplx;
long double _Imaginary imgry;

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
// DQ (7/8/2019): Old code, added name to union to avoid floating point exception.
// un-named unions for C were previously dropped (not present in the AST). This is
// the recent fix (which may be leading to this error).
//};
  } XYZ;
  U v[5];
  short u[5];
  char w[7];
  int t;
};
struct D h;


