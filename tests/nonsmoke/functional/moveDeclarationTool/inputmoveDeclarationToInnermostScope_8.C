// Test cases for if-stmt
// if-stmt is the innermost scope  in the scope tree
//  Two cases:
//  1. var decl can be further moved into true and false branch bodies
//      treat it as normal innermost common scope with multiple scopes
//  2. var decl cannot be moved further
//      if-stmt is the innermost scope to be inserted into. Must adjust to a higher level scope instead.
// used in multiple branches, can be moved
#include <stdio.h>

void foo(int cond, int* blockx, int loSt)
{
  int tmp ;

  // if-stmt , eligible to move into two branches
  if (cond) 
  { tmp = 6; printf("%d", tmp) ; }
  else 
  { tmp = 7 ; printf("%d", tmp) ; }  


  // another if-stmt case
  // variable is used in both true and false body
  // But not eligible to move into two bodies
  // Naive analysis will find if-stmt is the inner-most common scope.
  {
    int blockx;
    if (loSt)
      blockx ++;
    else
      blockx --;
   }

// static variable should not be moved
  static int stmp ;

  if (cond) 
  { stmp = 6; printf("%d", stmp) ; }
  else 
  { stmp = 7 ; printf("%d", stmp) ; }  


}

// A tricky case of if-stmt, 
// move to two bodies, trigger another round of moving: iterative moving
#if 0
void bar (bool allow)
{
  int i ;
  for (i=0; i<10; ++i) {

  }
  if (allow) {
    for (i=0; i<4; ++i) {
    }
  }
  else {
    for (i=0; i<4; ++i) {
    }
  }
}
#endif

