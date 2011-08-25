#include "faultToleranceArrayLib.h" 

int main()
{
  int i;
  int A[100UL];
  int B[100UL][100UL];
  int C[100UL];
  
#pragma mem_fault_tolerance ( Chunksize = 8 )
  for (i = 0; i < 10; i++) {
    A[i] = 0;
  }
  clearHashTable();
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > ) ( B : < 0 : 100 > < 0 : 100 > ) ( C : < 0 : 100 > ) ( Chunksize = 8 )
  updateArray("A",A + 0,0,100,8);
  updateArray("B",B[0] + 0,0,100,0,100,8);
  for (i = 0; i < 10; i++) {
    int _memTemp0 = B[i + 1][i + 5];
    B[i + 1][i + 5] = A[i];
    updateElem("B",B[i + 1][i + 5],i + 1,i + 5,_memTemp0,8);
    int _memTemp1 = C[i];
    C[i] = B[i][i + 2];
    updateElem("C",C[i],i,_memTemp1,8);
  }
  validateArray("A",A + 0,0,100,8);
  validateArray("B",B[0] + 0,0,100,0,100,8);
  clearHashTable();
  return 0;
}
