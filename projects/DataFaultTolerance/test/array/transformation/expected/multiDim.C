#include "faultToleranceArrayLib.h" 

int main()
{
  int A[100UL][100UL];
  int B[100UL][100UL];
  int i;
  int j;
  int temp;
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > < 0 : 100 > ) ( B : < 0 : 100 > < 0 : 100 > ) ( Chunksize = 8 )
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      int _memTemp0 = A[i][j];
      A[i][j] = (i + j);
      updateElem("A",A[i][j],i,j,_memTemp0,8);
      int _memTemp1 = B[i][j];
      B[i][j] = 0;
      updateElem("B",B[i][j],i,j,_memTemp1,8);
    }
  }
  clearHashTable();
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > < 0 : 100 > ) ( B : < 0 : 100 > < 0 : 100 > ) ( Chunksize = 8 )
  updateArray("A",A[0] + 0,0,100,0,100,8);
  for (i = 0; i < 100; i++) {
    for (j = 0; j < 100; j++) {
      int _memTemp2 = B[i][j];
      B[i][j] = A[j][i];
      updateElem("B",B[i][j],i,j,_memTemp2,8);
    }
  }
  validateArray("A",A[0] + 0,0,100,0,100,8);
  clearHashTable();
  return 0;
}
