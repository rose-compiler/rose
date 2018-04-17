
void
test_cpuid()
   {
     __asm__ ("    mov %%ebx, %%esi; " : : : "ecx", "edx", "esi", "memory" );
   }

