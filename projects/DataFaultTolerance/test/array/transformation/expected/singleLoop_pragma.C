#include "faultToleranceArrayLib.h" 

int main()
{
  int A[100UL];
  int i;
  
#pragma mem_fault_tolerance ( A : < 0 : 100 > ) ( Chunksize = 8 )
  for (i = 0; i < 100; i++) {
    int _memTemp0 = A[i];
    A[i] = 5;
    updateElem("A",A[i],i,_memTemp0,8);
  }
  clearHashTable();
  return 0;
}
