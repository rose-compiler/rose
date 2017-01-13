// switch_bug.cpp: bug in switch-case construct

// #include <stdio.h>
// #include <assert.h>

void foobar()
   {
     int i = 1;
     switch(i) 
        {
          static int s = 42;
          s = 0;      // unreachable code (which is put into the next case by mistake)
          case 1:
               i = s;
               break;
          case 2:
               i = 2;
               break;
          default:
               i = 99;
        }
   }

