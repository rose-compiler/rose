namespace CHSystem
   {
  // int memalign(void **a_memptr, size_t a_alignment, size_t a_size);

     class X
        {
          int memalign();
        };
   }

// This function will not be unparsed!
// Actually it appears to not be in the AST!!!
// int CHSystem::memalign(void **a_memptr, size_t a_alignment, size_t a_size)
int CHSystem::X::memalign()
   {
     return 0;
   }
