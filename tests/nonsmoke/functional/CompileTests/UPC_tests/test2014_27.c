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
  // Bug:   j =  *((int *)(((char *)(((SupLib *)(((char *)suplib) + MPISMOFFSET)) -> ((int *)(((char *)ptr) + MPISMOFFSET)))) + MPISMOFFSET));
  // This example has one more transformation than required.  The version of code before had one fewer than required.
     j = *(suplib->ptr);
#endif
#if 0
     shared int* j_ptr;
     j_ptr = (suplib->ptr);
#endif
#if 0
     suplib = 0L;
#endif
   }
