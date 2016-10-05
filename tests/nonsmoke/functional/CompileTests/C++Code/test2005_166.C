/*
This is the first bug from compiling ROSE with ROSE. Compiling this code:

gives the following output:

identityTranslator:
../../../../NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:18365:
SgForStatement* sage_gen_for_statement(a_statement*): Assertion
`isSgBasicBlock(sageStmt) != __null' failed.
Aborted
 */

void foo()
   {
     int j = 0;
     for ( int i=0; i<10; i++ ) 
          j++;
   }



#include <stdlib.h>

void N( int *argc \
     , char **argv \
     , const char *flind \
     , const char *assop \
     , const char *pname \
     , float *value \
     , int argd /* no default argument */ \
     ){
  char **rr, eol='\0';
  int i;
  rr = (char**)malloc( (*argc)*sizeof(char*) );
  for ( i=0; i<*argc; i++ ) rr[i] = &eol;
}
