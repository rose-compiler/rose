typedef struct 
   {
     shared int * ptr;
   } SupLib;

void subroutine(shared SupLib *suplib) 
   {
#if 0
     int i;
     i = *((*suplib).ptr);
#endif
#if 1
     int j;
     j = *(suplib->ptr);
#endif
#if 1
     shared int* j_ptr;
  /* "j_ptr" should not be transformed since it is an L-value. */
  /* I think this should be transformed to be:  "j_ptr = ((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ptr;" */
     j_ptr = (suplib->ptr);
#endif
#if 0
     shared int* k_ptr;
  /* "j_ptr" should not be transformed since it is an L-value. */
  /* I think this should be transformed to be:  "((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ptr = k_ptr;" */
  // k_ptr = (suplib->ptr);
     suplib->ptr = k_ptr;
#endif
#if 0
  /* I think this should be transformed to be:  "((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ptr = ((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ptr;" */
  // k_ptr = (suplib->ptr);
     suplib->ptr = suplib->ptr;
#endif
#if 0
  /* This should not be transformed since it is an L-value. */
     suplib = 0L;
#endif
   }

