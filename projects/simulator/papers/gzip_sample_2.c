/* Sample of code from gzip from util.c, with a couple minor changes:
 *
 *   1. The "crc" variable is local and used only to initialize "c".  In real gzip, many of the functions have global variable
 *      side effects, and this was one.
 *
 *   2. The crc_32_tab was a statically initialized, global array in gzip.  We replace it with a function here, although our
 *      function doesn't return the same values as in gzip.
 */

#include <stdio.h>
#include <string.h>

unsigned long
crc_32_tab(unsigned long seed)
{
    seed += (seed << 3);
    seed ^= (seed >> 11);
    seed += (seed << 15);
    return seed;
}

unsigned long
updcrc(const unsigned char *s, unsigned n)
{
    unsigned long c = 0xfffffffful;

    if (s) {
        while (n-->0) {
            c = crc_32_tab((c ^ *s++) & 0xff) ^ (c >> 8);
        }
    }
    return c ^ 0xffffffffL;
}

void
show_result(const char *s)
{
    unsigned long crc = updcrc((const unsigned char*)s, strlen(s));
    printf("s=\"%s\"; crc=0x%08lx\n", s, crc);
}

int
main()
{
#if 1 /* we don't actually need to ever call updcrc() */
    show_result("xxxx");
    show_result("t$xV");
#endif
    return 0;
}
