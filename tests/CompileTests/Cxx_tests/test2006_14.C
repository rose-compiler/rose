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


// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3)

#include <stdio.h>
#ifdef __cplusplus
  #include <iostream.h>
  #include <iomanip.h>
#else
  #include<string.h>
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

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

