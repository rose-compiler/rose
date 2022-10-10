#include <cstdlib>
using namespace std;

int main() {
  int* a[3];
  int* intp;
  int* intp2;
  int** pa;
  void* mpvoid;

  mpvoid=std::malloc(16);
  intp=(int*)mpvoid;
  (*intp)=100;
  *(intp+1)=120;
  a[0]=(int*)std::malloc(8);
  a[1]=intp;
  *a[1]=4000;
  pa=a; 
  // @pp:(a,0), @(a,0):($mem2,0)
  intp=*pa; 
  // p->($mem2,0)
  *intp=5000;
  **pa=6000;
  //(*p)++;
  //pp=&a[1];
  return 0;
}
