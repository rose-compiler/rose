#include "faultToleranceLib.C" 

int main()
{
  int i;
  int A[100UL][100UL];
  int B[100UL][100UL];
  int C[100UL];
  float Fl[100UL];
  
#pragma mem_fault_tolerance
  for (i = 0; i < 10; i++) {
    A[i][5] = 0;
    updateCheckBits(A[i] + 5,"int");
  }
  
#pragma mem_fault_tolerance
  for (i = 0; i < 10; i++) {
    verifyCheckBits(A[i] + 5,"int");
    verifyCheckBits(C + i,"int");
    B[i][5] = (A[i][5] + C[i]);
    updateCheckBits(B[i] + 5,"int");
  }
  return 0;
}
