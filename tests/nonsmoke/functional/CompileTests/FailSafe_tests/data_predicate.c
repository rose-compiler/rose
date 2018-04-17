#define MAXCYCLES 500
void foo()
{
  int ncycles;
#pragma failsafe data assert ((0 <= ncycles) && (ncycles <= 100)) 

}
