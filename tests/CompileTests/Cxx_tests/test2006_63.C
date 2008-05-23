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

float atof(char*);

/*** Sla for type T (automatically uses function M to modify). ***/
#define SLA_CNV(C) \
  char **rr, eol='\0'; \
  int i, nvalue; \
  rr = 0L; \
  for ( i=0; i<*argc; i++ ) rr[i] = &eol; \
  nvalue = 1; \
  if ( nvalue > 0 && value != 0L ) { \
    if ( *pname == '*' ) { \
      for ( i=0; i<nvalue; i++ ) value[i] = C(rr[i]); \
    } \
    else { \
      for ( i=0; i<nvalue; i++ ) value[0] = C(rr[i]); \
    } \
  } \
  return nvalue;

// #include<sla.h>

/*
  Generate additional sla for various primitive types.
  The use of the SLA macro makes it
  difficult to see the prototypes of the functions created.
  To find the prototypes, try this:
  > CC -P sla.cc
  > pgrep 's/^(int sla\s*\([^\{]*).*$/$1;/' sla.i -s
 */
// SLA_FCN_PROTO( sla_float, float )

void foo()
   {
     if ( true )
        {
          int x;
        }
   }

