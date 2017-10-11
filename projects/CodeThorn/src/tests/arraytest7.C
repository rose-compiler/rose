#include <cstdlib>
using namespace std;

int main() {
  int* a[3];
  int* p;
  int** pp;
  void* vp;

  vp=std::malloc(12);
  p=(int*)vp;
  //pp=&p; // error
  //**pp=3000;
  (*p)=100;
  *(p+1)=101;
  a[0]=(int*)std::malloc(4);
  a[1]=p;
  *a[1]=4000;
  pp=a; 
  // @pp:(a,0), @(a,0):($mem2,0)
  p=*pp; 
  // bug: p->(a,0) instead of p->($mem2,0)
  *p=5000;
  **pp=6000;
  //(*p)++;
  //pp=&a[1];
  return 0;
}
