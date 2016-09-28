// #include <stdio.h>

void foo(int cond, int* blockx, int loSt)
{
// static variable should not be moved
   static int stmp,a,b ;

  if (cond) 
  { stmp =    4; }
  else 
  { stmp =    5 ; }  

  int tmp ;

  // if-stmt , eligible to move into two branches
  if (cond) 
      {     tmp =    6; }
      else 
      {  tmp = 7; }  



}

