// #include <linux/types.h>
// #include <stdio.h>

// DQ (2/14/2014): Required for access to uint32_t on Rhel6 systems.
#include <stdint.h>

static inline void
test_cpuid_new(uint32_t i, uint32_t *buf)
{
    uint32_t  eax, ebx, ecx, edx, trash;

 // printf("Probe: %s\n", "NEW ALGORITHM");    
    /*
     * we could not use %ebx as output parameter if gcc builds PIC,
     * and we could not save %ebx on stack, because %esp is used,
     * when the -fomit-frame-pointer optimization is specified.
     */

    __asm__ (

    "    mov    %%ebx, %%esi;  "

    "    cpuid;                "
    "    mov    %%ebx, %%edi;  "

    "    mov    %%esi, %%ebx;  "

    : "=a" (eax), "=D" (ebx), "=c" (ecx), "=d" (edx), "=S" (trash) : "a" (i) );

    buf[0] = eax;
    buf[1] = ebx;
    buf[2] = edx;
    buf[3] = ecx;
}

