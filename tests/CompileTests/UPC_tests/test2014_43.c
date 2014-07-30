/* This is a subset of in16.c */

typedef struct 
   {
   } AngEng;

/* This is the simplest example of the bug. */
/* const shared AngEng * const AE_ptr; */
const shared AngEng * const AE_ptr;

#if 1
/* Original code: void foobar( const shared AngEng * const AE); */
void foobar( const shared AngEng * const AE);
#endif
