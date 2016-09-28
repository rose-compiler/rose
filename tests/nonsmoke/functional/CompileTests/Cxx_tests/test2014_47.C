
// typedef unsigned long size_t;

namespace CHSystem
   {
  // int memalign(void **a_memptr, size_t a_alignment, size_t a_size);
     int memalign();
   }

// This function will not be unparsed!
// Actually it appears to not be in the AST!!!
// int CHSystem::memalign(void **a_memptr, size_t a_alignment, size_t a_size)
int CHSystem::memalign()
   {
     return 0;
   }
