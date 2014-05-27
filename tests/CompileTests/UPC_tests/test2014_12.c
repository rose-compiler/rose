shared int * ptr;
int* unshared_ptr;

void subroutine() 
   {
     int i;

  // The code "i = *ptr;" should be transformed to be "i = *((int *)(((char *)(ptr))+MPISMOFFSET));"
     i = *ptr;
     i = *unshared_ptr;

     *ptr = i;
   }
