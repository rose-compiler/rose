#include "inputmoveDeclarationToInnermostScope_test2015_40.h"

void foobar() 
   {
     int e = 7;
     int d;
     int f,g,h;

     if (true) 
        {
          MACRO(d, 42, e);
        }
   }

