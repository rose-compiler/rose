//Example for status predicate
// 
extern int f0(int);
extern int g(int);
// Recovery routine
extern void R3(int, int ,int);
void foo(int U)
{

  int x, y, z;
  x = f0(y);
#pragma failsafe status assert (x < U) pre error (ET1) recover (R3,x,y,U)  
L1: z = g(x);

}
