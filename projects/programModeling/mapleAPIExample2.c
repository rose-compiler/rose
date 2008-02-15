/*
Notes (DQ (930/2005):
   This file shows an example of how to call Maple from a C program.
   The following line is what is requird to compile this code:
      gcc -I/usr/apps/maple/9/extern/include -fPIC mapleAPIExample2.c -o mapleAPIExample2 -L/usr/apps/maple/9/bin.IBM_INTEL_LINUX -lmaplec -lmaple
   A bug found in the execution is that the executable must be run as 
   "./mapleAPIExample2" instead of "mapleAPIExample2".  Because this 
   is non-intuative (and so that I will remember) the condition that 
   generates the error is warned about explicitly in the modification 
   of this example code below (see code). 

   From the output, I think that the output is supposed to be C source 
   code in some form, either that or there might also be a problem with 
   the "MapleALGEB_Printf()" function as well.  

   The output of this code is:

Calling startMaple ... 
Compute a derivative: 
        -4/(x^4+1)^2*x^3
printf("\nThe derivative at x=3 is: %a\n",-27/1681)

 */

#include <stdio.h>
#include <stdlib.h>

/* OpenMaple routines are defined here */
#include "maplec.h"
  
/* Callback used for directing result output */
static void M_DECL textCallBack( void *data, int tag, char *output )
   {
     printf("%s\n", output);
   }
     
int main( int argc, char *argv[] )
   {
     char err[2048];

  /* Maple kernel vector - used to start and make calls to Maple.  */
  /* All OpenMaple function calls take kv as an argument. */
     MKernelVector kv;

     MCallBackVectorDesc cb = {  textCallBack,
                                 0,   /* errorCallBack not used */
                                 0,   /* statusCallBack not used */
                                 0,   /* readLineCallBack not used */
                                 0,   /* redirectCallBack not used */
                                 0,   /* streamCallBack not used */
                                 0,   /* queryInterrupt not used */
                                 0    /* callBackCallBack not used */
                              };

  /* ALGEB is the C data type for representing Maple data structures */
     ALGEB r;

  // DQ (9/30/2005): This fixes a bug in the API for Maple (v9.0).  The bug is 
  // that Maple can't handle the value of argv[0] to be an empty string once 
  // the basename has been stripped off.  For now we just warn of this problem
  // and let the Maple function "StartMaple" fail directly.  We could fixup
  // argv[0] if this continues to be a poblem in Maple version 10.
     if (strlen(argv[0]) == strlen((char*)basename(argv[0])))
          {
            printf ("Error: Specify application using maple using \"./\" prefix as in ./%s (bug in Maple) \n",argv[0]);
          }

  /* Initialize Maple */
     printf ("Calling startMaple ... \n");
     if( ( kv = StartMaple(argc, argv, &cb, NULL, NULL, err) ) == NULL )
        {
          printf( "Could not start Maple, %s\n", err );
          exit(1);
        }
 
  /* Compute the derivative of 1/(x^4+1) */
  /* Output goes through the textCallBack() procedure */
     printf( "Compute a derivative: \n\t" );
     r = EvalMapleStatement( kv, "diff(1/(x^4+1), x);" );

  /* Evaluate the derivative at x = 3 */
     MapleAssign(kv,
                 ToMapleName( kv, "x", TRUE ),
                 ToMapleInteger( kv, 3 ));
  // r = MapleEval( kv, r );
     r = MapleEval( kv, r );
  // MapleALGEB_Printf( kv, "\nThe derivative at x=3 is: %a\n", r );
     MapleALGEB_Printf( kv, "\nThe derivative at x=3 is: %a\n", r );
  // printf("The derivative at x=3 is: %a \n",r);

     return 0;
   }

