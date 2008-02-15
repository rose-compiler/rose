#include "matvec.h"
#include "eigenvalue.h"
#include <vector>
#include <cstdlib>
#include <string>
#include <cassert>

using namespace std;

int main(int argc, char** argv) {
  int size = 100;
  vector<double> A(size * size);
  for (int i = 0; i < size * size; ++i)
    A[i] = drand48();

  double d = largest_eigenvalue(A);
  return 0;
}
