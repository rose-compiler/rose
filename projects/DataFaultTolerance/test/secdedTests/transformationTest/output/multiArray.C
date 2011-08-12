#include "faultToleranceLib.h" 

int main()
{
  int A[100UL];
  int B[100UL];
  int i;
  int temp;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    A[i] = 5;
    updateCheckBits(A + i,"int");
    B[i] = 0;
    updateCheckBits(B + i,"int");
  }
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    verifyCheckBits(B + i,"int");
    verifyCheckBits(A + i,"int");
    B[i] = (B[i] + A[i]);
    updateCheckBits(B + i,"int");
  }
  return 0;
}
