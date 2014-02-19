
void
test_cpuid()
   {
  // Original code:
  // __asm__ ("    mov %%ebx, %%esi; " : : : "ecx", "edx", "esi", "memory" );
     __asm__ ("    mov %%ebx, %%esi; " : : : "ecx", "edx", "esi", "memory" );
   }

