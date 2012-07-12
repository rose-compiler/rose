#include "faultToleranceArrayLib.h" 

int main()
{
  int A[100UL];
  int i;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) 
    A[i] = 5;
  clearHashTable();
  return 0;
}
