#if 0
The following code:
void foo(){
  char * flind;
  strlen(flind);

};
gives the following error when compiled with ROSE:
"../../../../ROSE/src/util/commandlineProcessing/sla.c", line 5: error:
          identifier "strlen" is undefined
    strlen(flind);

It compiles fine with GCC.

#endif

#include <stdio.h>
#ifdef __cplusplus
#include <iostream.h>
#include <iomanip.h>
#endif

#include <strings.h>
#include <unistd.h>

#if defined(J90)/*Cray*/ || defined(C90)/*Cray*/ || defined(IP)/*SGI*/
#include <sys/types.h>
#endif

// We don't include string.h directly but it
// is included from the header files above.
// #include<string.h>

void foo()
   {
     char * flind;
     strlen(flind);
   };
