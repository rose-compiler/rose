/*
 * bug 368
 * https://outreach.scidac.gov/tracker/index.php?func=detail&aid=368&group_id=24&atid=185
 *
 * ROSE (or EDG) tries to be smart when a possible out of bound pointer access is encountered.
 * e.g.
 * double x[NUM]; 
 * if x -1 is used, the compiler will change it to x and issue a warning: 
 * pointer points outside of underlying object
 * 
 * But it actually changes the original semantics of correct code. 
 * This bug causes EP of the NPB 2.3 OpenMP C version benchmark to fail.
 *
 * Liao, 9/22/2009
 * */
#include <assert.h>
static double x[1];
void vranlc(double* y)
{
  (y+1)[0] = 0.5;
};

int main()
{
  vranlc(x-1);
 /* 
   vranlc(x); // wrong translation!!
   */
  assert (x[0]==0.5);
  return 0;
}

