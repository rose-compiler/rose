#include<iostream>
#include<fstream>
using namespace std;

#define A_ a_data[i]
#define B_ b_data[i]
#define C_ c_data[i]
#define D_ d_data[i]
#define E_ e_data[i]


inline void calc(double* a_data, double* b_data, double* c_data,
                 double* d_data, double* e_data, int N){
  int N_ = N, i=0;
  for(i=0; i < N_; ++i) {
    A_ = ( 
// #include "expression.code"
  A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ 

+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
#if 0
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
+ A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_ + A_
#endif
         );
  }
}


int main() {
  ifstream PARAMETER;
  double *a_data, *b_data, *c_data, *d_data, *e_data;
  int N = 10, iter = 20, cnt,i ;

#if 0
  PARAMETER.open("para.dat",ios :: in);
  PARAMETER >> N >> iter;
  PARAMETER.close();
#endif

  a_data = new double[N];
  b_data = new double[N];
  c_data = new double[N];
  d_data = new double[N];
  e_data = new double[N];

  int N_ = N;
  for(i=0; i < N_; ++i) {
    a_data[i] = 0.0;
    b_data[i] = 3.0;
    c_data[i] = 5.0;
    d_data[i] = 7.0;
    e_data[i] = 9.0;
  }

  cnt = 0;
  marke:
    calc(a_data, b_data, c_data, d_data, e_data, N);
  cnt++;
  if(cnt < iter)
    goto marke;

  delete[] a_data;
  delete[] b_data;
  delete[] c_data;
  delete[] d_data;
  delete[] e_data;

  return 0;
}
