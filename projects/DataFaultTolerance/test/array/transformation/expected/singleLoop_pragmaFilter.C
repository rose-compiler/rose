#include "faultToleranceArrayLib.h" 

int main()
{
  int A[100];
  int i;
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) 
    A[i] = 5;
  clearHashTable();
}
