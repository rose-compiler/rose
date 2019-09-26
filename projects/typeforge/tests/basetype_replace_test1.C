typedef double** MULTIPOINTER;
typedef MULTIPOINTER ARRAYOFPOINTER[3][4][5];
typedef ARRAYOFPOINTER*** POINTERTOP;
typedef POINTERTOP TOPLEVEL[3][4][5];

double  ga = 0.0;
double* gb = &ga;
int     gc = 0;

class A{
};

class B{
};

A gca;
B gcb;

void f0(double& a){
  double     b;
  double*    c;
  double**** d;
  double e[4];
  double f[6][5][4];
  double* (*g)[7][8];
  TOPLEVEL h;
  const double i = 5;
  double* restrict j;
  volatile double k;
}

void f1(double& a){
  double     b;
  double*    c;
  double**** d;
  double e[4];
  double f[6][5][4];
  double* (*g)[7][8];
  TOPLEVEL h;
  const double i = 5;
  double* restrict j;
  volatile double k;
}

double f2(double& a){
  double     b;
  double*    c;
  double**** d;
  double e[4];
  double f[6][5][4];
  double* (*g)[7][8];
  TOPLEVEL h;
  const double i = 5;
  double* restrict j;
  volatile double k;
  return 0.0;
}

int main() {
  return 0;
}
