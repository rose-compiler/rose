/* This is a subset of in16.c */

typedef struct 
   {
   } AngEng;

/* This is the simplest example of the bug. */
/* const shared AngEng * const AE_ptr; */
// const shared AngEng * const AE_ptr;
const shared AngEng * const AE_ptr;
