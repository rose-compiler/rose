#include <cassert>

int main() {
  int a[3];
  int i=1;
  int* p=&a[0];
  a[0]=a[1]=a[2]=1;
  a[a[0]+0]=a[i+1]+1;
  *(p+1)=*(p+1);
  (int)(i=10);
  assert(a[0]==1 && a[1]==2);
  return 0;
}
