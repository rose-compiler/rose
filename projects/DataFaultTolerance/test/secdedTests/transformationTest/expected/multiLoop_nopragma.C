#include "faultToleranceLib.h" 

int main()
{
  int A[100UL];
  int i;
  int temp;
  for (i = 0; i < 100; i++) 
    A[i] = 5;
  for (i = 0; i < 100; i++) {
    temp = A[i];
  }
  return 0;
}
