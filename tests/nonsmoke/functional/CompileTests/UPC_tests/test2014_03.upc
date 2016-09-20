// Transform: "shared int * shared * pptr;" to be "int **pptr;"
shared int * shared * pptr;
// int ** nonshared_pptr;

#if 1
void subroutine() 
   {
     int i;

  // Transform "i = **pptr;" to be:
  // i = *((int *)(((char *)(*((int **)(((char *)(pptr))+MPISMOFFSET))))+MPISMOFFSET));
     i = **pptr;

//   **pptr = i;

//   i = *((int *)(((char *) (*((int **)(((char *)(nonshared_pptr))+42))))+42));
   }
#endif
