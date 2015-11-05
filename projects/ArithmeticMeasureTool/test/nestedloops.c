#include <stdio.h>
#define SIZE 10 

// Instrumentation 1: add a few global variables
unsigned long int chiterations = 0;
unsigned long int chloads = 0;
unsigned long int chstores = 0;
unsigned long int chflops = 0;

double ref[2] = {9.2, 5.4};
double coarse[SIZE][SIZE][SIZE];
int main()
{
  double refScale = 1.0 / (ref[0] * ref[1]);
  int iboxlo1 = 0, iboxlo0 = 0, iboxhi1 = SIZE-1, iboxhi0 = SIZE-1; 
  int var; 
  int ic1=0, ic0=0;
  int ip0 = ic0 * ref[0];
  int ip1 = ic1 * ref[1];
  double coarseSum = 0.0;
  int ii1, ii0;

  for (var =0; var < SIZE ; var++)
  {
    //Instrumentation 2: pass in loop iteration for the loop to be counted
    chiterations = (1 + iboxhi1 - iboxlo1) * (1 + iboxhi0 - iboxlo0) * 1;
    for (ic1 = iboxlo1; ic1< iboxhi1 +1; ic1++)
      for (ic0 = iboxlo0; ic0< iboxhi0 +1; ic0++)
      {
        int ibreflo1 = 0, ibreflo0 = 0, ibrefhi1 = SIZE-1, ibrefhi0 = SIZE-1; 
        //Instrumentation 3: pass in loop iteration for the loop to be counted
        chiterations = (1 + ibrefhi1 - ibreflo1) * (1 + ibrefhi0 - ibreflo0) * 1;
        for (ii1 = ibreflo1; ii1< ibrefhi1 +1; ii1++)
          for (ii0 = ibreflo0; ii0< ibrefhi0 +1; ii0++)
          {
            coarseSum = coarseSum +  coarse[ii1][ii0][ii1] +(ip0 + ii0) + (ip1 + ii1)  + var;
          }
        coarse[ic0][ic1][var] = coarseSum * refScale;
      }
  }
  //Instrumentation 4: pass in loop iteration for the loop to be counted
  printf ("chflops =%lu chloads =%lu chstores=%lu\n", chflops, chloads, chstores);
  return 0;
}
