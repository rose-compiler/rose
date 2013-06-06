#if defined(__i386__)
#define cpuid(a, b, c, d, inp) \
  asm("mov %%ebx, %%edi\n"     \
      "cpuid\n"                \
      "xchg %%edi, %%ebx\n"    \
      : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
#elif defined(__x86_64__)
#define cpuid(a, b, c, d, inp) \
  asm("mov %%rbx, %%rdi\n"     \
      "cpuid\n"                \
      "xchg %%rdi, %%rbx\n"    \
      : "=a" (a), "=D" (b), "=c" (c), "=d" (d) : "a" (inp))
#else
#define cpuid(a, b, c, d, inp)
#endif

#define uint32_t unsigned int

void AtomicOps_Internalx86CPUFeaturesInit()
   {
     uint32_t eax;
     uint32_t ebx;
     uint32_t ecx;
     uint32_t edx;

  // Get vendor string (issue CPUID with eax = 0)
  // Unparses as: 
  //    asm volatile ("mov %%rbx, %%rdi\\ncpuid\\nxchg %%rdi, %%rbx\\n" : "=a" (eax), "=D" (ebx), "=c" (ecx), "=d" (edx) : "a" (0));
  // But should be unparsed as:
  //    asm("mov %%rbx, %%rdi\n" "cpuid\n" "xchg %%rdi, %%rbx\n" : "=a" (eax), "=D" (ebx), "=c" (ecx), "=d" (edx) : "a" (0));
     cpuid(eax, ebx, ecx, edx, 0);
   }
