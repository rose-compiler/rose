#include "faultToleranceLib.h" 

int main()
{
  int A[100UL][100UL];
  int B[100UL][100UL];
  int i;
  int j;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      A[i][j] = (i + j);
      updateCheckBits(A[i] + j,"int");
      B[i][j] = 0;
      updateCheckBits(B[i] + j,"int");
    }
  }
// no error
  B[10][10] = 15;
//1-bit error
  A[10][10] = 21;
//2-bit error
  A[30][30] = 63;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      verifyCheckBits(A[j] + i,"int");
      B[i][j] = A[j][i];
      updateCheckBits(B[i] + j,"int");
    }
  }
  return 0;
}
