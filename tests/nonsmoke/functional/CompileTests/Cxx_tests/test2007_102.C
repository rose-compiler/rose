/*
When compiling the attached files I get the following error:

rose_rose_pldhash.c:5: error: conflicting types for 'PL_DHashTableOperate'
pldhash2.h:2: error: previous declaration of 'PL_DHashTableOperate' was here

this is due to __attribute(...)__ not unparsing. Seems like we are
getting more and more of these two file unparsing errors.

Andreas 
*/

// #include "pldhash.h"

void * __attribute__ ((regparm (3),stdcall)) PL_DHashTableOperate(int *table, const void *key, int op);

void*  __attribute__ ((regparm (3),stdcall)) /*PL_DHASH_FASTCALL*/ PL_DHashTableOperate(int *table,const void *key,int op)
{
  return 0;
 }

