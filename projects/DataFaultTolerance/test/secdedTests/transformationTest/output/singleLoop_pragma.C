#include "faultToleranceLib.h" 

int main()
{
  int A[100UL];
  int i;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    A[i] = 5;
    updateCheckBits(A + i,"int");
  }
  return 0;
}
