#include <cstdlib>
using namespace std;

int main() {
  int* a[3];
  int* intp;
  int** pa;
  void* mpvoid;

  mpvoid=std::malloc(12);
  intp=(int*)mpvoid;
  //pp=&p; // error
  //**pp=3000;
  (*intp)=100;
  *(intp+1)=101;
  a[0]=(int*)std::malloc(4);
  a[1]=intp;
  *a[1]=4000;
  pa=a; 
  // @pp:(a,0), @(a,0):($mem2,0)
  intp=*pa; 
  // bug: p->(a,0) instead of p->($mem2,0)
  *intp=5000;
  **pa=6000;
  //(*p)++;
  //pp=&a[1];
  return 0;
}
