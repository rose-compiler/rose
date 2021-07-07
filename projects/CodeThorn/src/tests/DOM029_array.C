/* create dynamic 2-dim array (values for dimension are in the
   program), assigns value to every element, and deletes array */
#include <cstdio>

int main() {
  int m=3;
  int a[3];
  // array index
  for(int i=0;i<m;i++) {
    a[i]=i;
  }
  for(int i=0;i<m;i++) {
    printf("a[%d]=%d\n",i,a[i]); // read
  }
  for(int i=0;i<m;i++) {
    a[i]=a[i]+1;
  }
  for(int i=0;i<m;i++) {
    printf("a[%d]=%d\n",i,a[i]); // read
  }
  // pointer arithmetic
  int* p=&a[0];
  for(int i=0;i<m;i++) {
    *p=(*p)+1;
    p++;
  }
  for(int i=0;i<m;i++) {
    printf("a[%d]=%d\n",i,a[i]); // read
  }
  for(int i=m-1;i>=0;i--) {
    --p;
    *p=(*p)+1;
  }
  for(int i=0;i<m;i++) {
    printf("a[%d]=%d\n",i,a[i]); // read
  }
  return 0;
}
