// A test case showing the need for liveness analysis
// Straightline analysis will say it is safe to move int offset into the loop body.
//  definition --> use. 
// HOwever, offset is redefined in the end of the loop. There another def-use live path involved.
// The move should not alter the meet points of two live paths.
#include <stdio.h>
 
void foo(int *test, int *array, int length)
{
   int offset = 0;
   for (int j=0; j<length ; ++j) {
      if (test[j]) {
         array[offset] = j ;
         ++offset ;
      }
   }
}


void bar()
{
  int i = 0; 
  for (int j=10; j>0 ; --j) {
    printf("%d, %d\n", i, j) ;
    ++i ;             
  }                      
}

// with or without initializer, we cannot move it. 
void bar2()
{
  int i; 
  for (int j=10; j>0 ; --j) {
    printf("%d, %d\n", i, j) ;
    ++i ;             
  }                      
}

