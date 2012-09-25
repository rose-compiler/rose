/*
  Test vectorization for simple arithemetic statements.
*/
#include <math.h>
int main(){
  float a[16];
  float b[16];
  int n = 16;
  for (int i=0;i<n;i++)
  {
    b[i] = sin(a[i]);
    b[i] = cos(a[i]);
    b[i] = log(a[i]);
    b[i] = exp(a[i]);
    b[i] = sqrt(a[i]);
    b[i] = max(a[i],b[i]);
    b[i] = min(a[i],b[i]);
  }
}
