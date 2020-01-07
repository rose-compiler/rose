#include <iostream>
using namespace std;
// non-template version

int iGetMax (int a, int b) {
  int result;
#pragma rose_outline
  result = (a>b)? a : b;
  return result;
}


#if 1
template<typename T> // better choice
//template <class T>
T GetMax (T a, T b) {
  T result;
#pragma rose_outline
  result = (a>b)? a : b;
  return (result);
}


int main () {
  int i=5, j=6, k;
  long l=10, m=5, n;
  k=GetMax<int>(i,j);
  n=GetMax<long>(l,m);
  cout << k << endl;
  cout << n << endl;
  return 0;
}
#endif
