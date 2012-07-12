#include "faultToleranceLib.h" 

int main()
{
  int i;
  int j;
  int A[100UL][100UL];
  int B[100UL][100UL];
  int C[100UL];
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      A[i][5] = 0;
      updateCheckBits(A[i] + 5,"int");
    }
    B[i][5] = 5;
    updateCheckBits(B[i] + 5,"int");
  }
  
#pragma mem_fault
  for (i = 0; i < 100; i++) {
    A[i][5] = (B[i][5] + C[i]);
  }
  return 0;
}
