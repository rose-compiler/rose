/*
gives the following error:
 /home/saebjornsen2/ROSE-October/gcc3.3.3/exampleTranslators/documentedExamples/simpleTranslatorExamples/identityTranslator
-rose:C_only   -c
../../../../ROSE/src/util/commandlineProcessing/minimal.c -o sla.o
Wrapping the non-basic block statement in a SgBasicBlock before inserting
as loop body of for loop!
This case does not appear to be executed
identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18377:
SgForStatement* sage_gen_for_statement(a_statement*): Assertion `false'
failed.
Aborted

Seems like I made the example too slim earlier.

Thanks
Andreas

Compiling the code:

*/


/*** generates code for sla prototypes ***/
#define SLA_FCN_PROTO(N,T) \
int N( int *argc \
     , char **argv \
     , const char *flind \
     , const char *assop \
     , const char *pname \
     , T *value \
     , int argd /* no default argument */ \
     )

/*** For allocating and deleting memory ***/
#ifndef __cplusplus
#include <stdlib.h>
#endif


/*** Sla for type T (automatically uses function M to modify). ***/
#define SLA_CNV(C) \
  char **rr, eol='\0'; \
  int i, nvalue; \
  rr = (char**)malloc( (*argc)*sizeof(char*) ); \
  for ( i=0; i<*argc; i++ ) rr[i] = &eol; \
  nvalue = sla_str( argc, argv, flind, assop, pname, rr, argd ); \
  if ( nvalue > 0 && value != NULL ) { \
    if ( *pname == '*' ) { \
      for ( i=0; i<nvalue; i++ ) value[i] = C(rr[i]); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) value[0] = C(rr[i]); \
    } \
  } \
  free(rr); \
  return nvalue;

#include<sla.h>

/*
  Generate additional sla for various primitive types.
  The use of the SLA macro makes it
  difficult to see the prototypes of the functions created.
  To find the prototypes, try this:
  > CC -P sla.cc
  > pgrep 's/^(int sla\s*\([^\{]*).*$/$1;/' sla.i -s
 */
SLA_FCN_PROTO( sla_float, float )
   {
     SLA_CNV(atof) 
   }

