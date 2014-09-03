// // DQ (2/14/2014): Required for access to uint32_t and u_char on Rhel5 systems.
// #include <linux/types.h>

/* RPM (3/26/2014): C99 fixed width types are in either <inttypes.h> or <stdint.h> on Debian systems. "u_char" is not
 * a standard type and not defined in <inttypes.h> or <stdint.h>, at least not on Debian Squeeze, so I've replaced
 * u_char below with "unsigned char". */
#include <stdint.h>


#if 0
// DQ (2/22/2014): These are required for RedHat 6, but are redundant for RedHat 5 and cause an error.
// The solution has been to build a special RedHat 6 version of this test and to used an automake
// conditional to include one or the other in the regression tests.  This will allow the Jenkins
// tests on RedHat 5 to pass, while defining a way for the newer RedHat 6 tests to pass (that are
// not yet a part of Jenkins).  Where this file is used in other tests in ROSE it will have to be
// modified when we move to RedHat 6 testing using Jenkins.

// DQ (2/14/2014): Required for access to uint32_t on Rhel6 systems.
#include <stdint.h>

// DQ (2/14/2014): Required for access to u_char on Rhel6 systems.
#include <sys/types.h>
#endif

// This is at least what one would expect to be required!
#include <stdio.h>

#define test_strcmp(s1, s2)  strcmp((const char *) s1, (const char *) s2)

static inline void test_cpuid(uint32_t i, uint32_t *buf);
static inline void test_cpuid_new(uint32_t i, uint32_t *buf);


static inline void
test_cpuid_new(uint32_t i, uint32_t *buf)
{
    uint32_t  eax, ebx, ecx, edx, trash;

    printf("Probe: %s\n", "NEW ALGORITHM");    
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



static inline void
test_cpuid(uint32_t i, uint32_t *buf)
{
    printf("Probe: %s\n", "ORIGINAL ALGORITHM");

    /*
     * we could not use %ebx as output parameter if gcc builds PIC,
     * and we could not save %ebx on stack, because %esp is used,
     * when the -fomit-frame-pointer optimization is specified.
     */

    __asm__ (

    "    mov    %%ebx, %%esi;  "

    "    cpuid;                "
    "    mov    %%eax, (%1);   "
    "    mov    %%ebx, 4(%1);  "
    "    mov    %%edx, 8(%1);  "
    "    mov    %%ecx, 12(%1); "

    "    mov    %%esi, %%ebx;  "

    : : "a" (i), "D" (buf) : "ecx", "edx", "esi", "memory" );
}




/* auto detect the L2 cache line size of modern and widespread CPUs */

void
test_cpuinfo(void)
{
    unsigned char *vendor;
    uint32_t   vbuf[5], cpu[4], model, ngx_cacheline_size;

    ngx_cacheline_size = 0;

    vbuf[0] = 0;
    vbuf[1] = 0;
    vbuf[2] = 0;
    vbuf[3] = 0;
    vbuf[4] = 0;

    test_cpuid(0, vbuf);

    vendor = (unsigned char *) &vbuf[1];

    if (vbuf[0] == 0) {
        return;
    }

    test_cpuid(1, cpu);

    if (test_strcmp(vendor, "GenuineIntel") == 0) {
        printf("Processor Type: %s\n", "GenuineIntel");
        switch ((cpu[0] & 0xf00) >> 8) {

        /* Pentium */
        case 5:
            printf("Processor Model: %s\n", "Pentium");
            ngx_cacheline_size = 32;
            break;

        /* Pentium Pro, II, III */
        case 6:
            ngx_cacheline_size = 32;

            model = ((cpu[0] & 0xf0000) >> 8) | (cpu[0] & 0xf0);

            if (model >= 0xd0) {
                /* Intel Core, Core 2, Atom */
                printf("Processor Model: %s\n", "Core, Core 2, Atom");
                ngx_cacheline_size = 64;
            } else {
                printf("Processor Model: %s\n", "Pentium Pro, II, III");
            }

            break;

        /*
         * Pentium 4, although its cache line size is 64 bytes,
         * it prefetches up to two cache lines during memory read
         */
        case 15:
            printf("Processor Model: %s\n", "Pentium 4");
            ngx_cacheline_size = 128;
            break;
        }

    } else if (test_strcmp(vendor, "AuthenticAMD") == 0) {
        ngx_cacheline_size = 64;
    }
}

int main(int argc, char** argv) {
    test_cpuinfo();
}
