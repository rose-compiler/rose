#include <vector>
#include "matvec.h"
#include <iostream>

using namespace std;

extern "C"
void matvec(vector<double>& x, const vector<double>& A, 
	    const vector<double>& y) {
  cout << "Interchanged matvec" << endl;
  for (int i = 0; i < x.size(); ++i)
    x[i] = 0.;
  for (int j = 0; j < y.size(); ++j)
    for (int i = 0; i < x.size(); ++i) {
      x[i] += A[j * x.size() + i] * y[j];
  }
}
