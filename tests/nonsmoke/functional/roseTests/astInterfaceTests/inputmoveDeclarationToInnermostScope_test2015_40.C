// #define MACRO(a, b, c) c = a + b; c = a * b;
// void MACRO(int,int,int);

#include "inputmoveDeclarationToInnermostScope_test2015_40.h"

void foobar() 
   {
     int d = 7;
     int e,f,g;

     if (true) 
        {
          MACRO(d, 42, e);
        }
   }

