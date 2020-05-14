/* create dynamic 2-dim array (values for dimension are in the
   program), assigns value to every element, and deletes array */
#include <cstdio>

int main() {
  int** arr;
  int m=3;
  int n=4;
  arr=new int*[m];
  for(int i=0;i<m;i++) {
    arr[i]=new int [n]; // alloc
  }
  for(int i=0;i<m;i++) {
    for(int j=0;j<n;j++) {
      arr[i][j]=(i+1)*(j+1); // write
      printf("%d\n",arr[i][j]); // read
    }
  }
  for(int i=0;i<m;i++) {
    delete[] arr[i]; // dealloc
  }
  delete[] arr; // dealloc
  return 0;
}
