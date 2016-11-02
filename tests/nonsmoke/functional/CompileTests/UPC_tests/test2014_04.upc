// Transform: "shared int * shared * shared * ppptr;" to be "int ***ppptr;"
// shared int * shared * pptr;
shared int * shared * shared * ppptr;

#if 1
void subroutine() 
   {
     int i;

  // Transform "i = ***ppptr;" to be:
  // i = *((int *)(((char *)(*((int **)(((char *)(*((int ***)(((char *)(ppptr))+MPISMOFFSET))))+MPISMOFFSET))))+MPISMOFFSET));
     i = ***ppptr;

     ***ppptr = i;

  // Examples of what should be generated:
  // i = *((int *)(((char *)(*((int **)(((char *)(*((int ***)(((char *)(ppptr))+MPISMOFFSET))))+MPISMOFFSET))))+MPISMOFFSET));
  // *((int *)(((char *)(*((int **)(((char *)(*((int ***)(((char *)(ppptr))+MPISMOFFSET))))+MPISMOFFSET))))+MPISMOFFSET)) = i;
   }
#endif
