#include <stdio.h>

void foo(int cond, int* blockx, int loSt)
{
// static variable should not be moved
   static int stmp,a,b ;

  if (cond) 
  { stmp =    4; printf("%d", stmp) ; }
  else 
  { stmp =    5 ; printf("%d", stmp) ; }  

  int tmp ;

  // if-stmt , eligible to move into two branches
  if (cond) 
      {     tmp =    6; printf("%d", tmp) ; }
      else 
      {  tmp = 7; printf("%d", tmp) ; }  



}

