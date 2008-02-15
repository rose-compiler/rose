
#include<stdio.h>

double a;
float b;
#pragma "I am a Pragma"
float c = 3.14 + 1.0;

#pragma "I am a Pragma"
namespace X
   {
  // Here is a comment
     double a;
     float b;
     float c = 3.14;
   }


void foo()
   {
#pragma "I am a Pragma"
     double a;
     float b;
     float c = 3.14;
#pragma "I am a Pragma"
     a = b + c;
     if (true)
        {
          double a;
          float b;
          float c = 3.14;
        }
       else
        {
          double a;
          float b;
          float c = 3.14;
        }
   }
