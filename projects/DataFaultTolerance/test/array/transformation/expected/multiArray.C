#include "faultToleranceArrayLib.h" 

int main()
{
  int A[100UL];
  int B[100UL];
  int i;
  int temp;
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > ) ( B : < 0 : 100 > ) ( Chunksize = 10 )
  for (i = 0; i < 100; i++) {
    int _memTemp0 = A[i];
    A[i] = 5;
    updateElem("A",A[i],i,_memTemp0,10);
    int _memTemp1 = B[i];
    B[i] = 0;
    updateElem("B",B[i],i,_memTemp1,10);
  }
  clearHashTable();
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > ) ( B : < 0 : 100 > ) ( Chunksize = 10 )
  updateArray("A",A + 0,0,100,10);
  updateArray("B",B + 0,0,100,10);
  for (i = 0; i < 100; i++) {
    int _memTemp2 = B[i];
    B[i] = (B[i] + A[i]);
    updateElem("B",B[i],i,_memTemp2,10);
  }
  validateArray("A",A + 0,0,100,10);
  validateArray("B",B + 0,0,100,10);
  clearHashTable();
  return 0;
}
