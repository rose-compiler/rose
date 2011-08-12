#include "faultToleranceLib.C" 

int main()
{
  int i;
  int A[100UL][100UL];
  int B[100UL][100UL];
  int C[100UL];
  float Fl[100UL];
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    A[i][5] = 0;
    updateCheckBits(A[i] + 5,"int");
    B[i][5] = 5;
    updateCheckBits(B[i] + 5,"int");
    //Fl[i] = 10.99;
    //updateCheckBits(Fl + i,"float");
  }
  
#pragma mem_fault_tolerance
  for (i = 0; i < 100; i++) {
    verifyCheckBits(B[i] + 5,"int");
    verifyCheckBits(C + i,"int");
    A[i][5] = (B[i][5] + C[i]);
    updateCheckBits(A[i] + 5,"int");
  }
  
/*#pragma mem_fault_tolerance
  for (i = 0; i < 99; i++) {
    verifyCheckBits(Fl + (i + 1),"float");
    Fl[i] = Fl[(i + 1)];
    updateCheckBits(Fl + i,"float");
  }*/
  return 0;
}
