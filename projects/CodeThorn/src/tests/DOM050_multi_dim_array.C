/* create dynamic 2-dim array (values for dimension are in the
   program), assigns value to every element, and deletes array */
#include <cstdio>

int main() {
  int m=3, n=4;
  int a[3][4];
  
  for(int i=0;i<m;i++) {
    for(int j=0;j<n;j++) {
      a[i][j]=100; //(i+1)*(j+1); // write
    }
  }
#if 0
  for(int i=0;i<m;i++) {
    for(int j=0;j<n;j++) {
      printf("a[%d,%d]=%d\n",i,j,a[i][j]); // read
    }
  }
#endif

  return 0;
}
