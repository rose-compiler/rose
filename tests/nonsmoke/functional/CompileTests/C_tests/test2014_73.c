
#include "test2014_73.h"

char*
update_path (const char *key)
   {
  // In the macro expansion, CONST_CAST(char *, key) will expand to:
  // char* nonconst_key = ((__extension__(union {const char * _q; char * _nq;})((key)))._nq);
     char* nonconst_key = CONST_CAST (char *, key);

  // free (CONST_CAST (char *, key));

     return nonconst_key;
   }
