#include<stdalign.h>
// void *aligned_alloc(size_t algn, size_t size);

void foo()
   {
  // Test use of the aligned_alloc() function defined in stdalign.h
     void* memory = aligned_alloc(1,2);
   }



