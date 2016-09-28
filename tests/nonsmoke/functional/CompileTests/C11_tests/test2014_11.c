
typedef unsigned long size_t;

void foo()
   {
     const int x = 0;
     _Static_assert(1,"Not a standard compliant compiler");

     _Static_assert(sizeof(size_t) >= 8, "size_t must be at least 64-bits");

  // This might be a machine architecture dependent tests.
     _Static_assert(sizeof(void *) == sizeof(void(*)()), "object pointer must be the same size as function pointer");
   }
