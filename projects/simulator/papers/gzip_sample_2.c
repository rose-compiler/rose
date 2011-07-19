/* Sample of code from gzip from util.c, with a couple minor changes:
 *
 *   1. The "crc" variable is local and used only to initialize "c".  In real gzip, many of the functions have global variable
 *      side effects, and this was one.
 *
 *   2. The crc_32_tab table is a local array and initialized each time we call this function.  The original code initialized
 *      this table, a global array, statically.
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

int
main()
{
#if 1 /* we don't actually need to ever call updcrc() */
    const char *s = "xx";
    size_t n = strlen(s);
    unsigned long crc = updcrc((const unsigned char*)s, n);
    printf("s=\"%s\"; crc=0x%08lx\n", s, crc);
#endif
    return 0;
}
