#include "faultToleranceLib.h" 

int main()
{
  int A[100UL];
  int i;
  int temp;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    A[i] = 5;
    updateCheckBits(A + i,"int");
  }
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    verifyCheckBits(A + i,"int");
    temp = A[i];
  }
  return 0;
}
