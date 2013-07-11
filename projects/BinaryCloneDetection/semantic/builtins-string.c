// A stand-in for the 32-bit standard C library
//
// The functions here attempt to make the same memory references (in terms of how many and what size) as
// the code emitted for GCC's built-in versions of these functions.

void
abort()
{
    __asm__("hlt");
}

void
__assert_fail()
{
    __asm__("hlt");
}

void *
memset(unsigned char *dst, int c, unsigned nbytes)
{
    unsigned at = 0;
    unsigned cb = c & 0xff;
    unsigned cw = (c<<24) | (c<<16) | (c<<8) | c;
    while (at+4 < nbytes) {
        *(int*)(dst+at) = cw;
        at += 4;
    }
    while (at < nbytes)
        dst[at++] = cb;
    return dst;
}

void *
memcpy(unsigned char *dst, unsigned char *src, unsigned nbytes)
{
    if (dst!=src) {
        unsigned at = 0;
        while (at+4 < nbytes) {
            *(unsigned*)(dst+at) = *(unsigned*)(src+at);
            at += 4;
        }
        while (at < nbytes) {
            dst[at] = src[at];
            ++at;
        }
    }
    return dst;
}

void *
memmove(unsigned char *dst, unsigned char *src, unsigned nbytes)
{
    if (dst == src) {
        /*void*/
    } else if (dst+4 <= src) {
        memcpy(dst, src, nbytes);
    } else if (dst < src) {
        int i;
        for (i=0; i<nbytes; ++i)
            dst[i] = src[i];
    } else {
        if (src+4 <= dst) {
            while (nbytes>=4) {
                nbytes -= 4;
                *(unsigned*)(dst+nbytes) = *(unsigned*)(src+nbytes);
            }
        }
        while (nbytes>0) {
            --nbytes;
            dst[nbytes] = src[nbytes];
        }
    }
    return dst;
}

char *
strcpy(char *dst, const char *src)
{
    while (1) {
        char c = *src++;
        *dst++ = c;
        if (!c)
            break;
    }
    return dst;
}

char *
strncpy(char *dst, const char *src, unsigned nbytes)
{
    unsigned at = 0;
    for (at=0; at<nbytes && src[at]; ++at)
        dst[at] = src[at];
    for (/*void*/; at<nbytes; ++at)
        dst[at] = '\0';
    return dst;
}

char *
strpncpy(char *dst, const char *src, unsigned nbytes)
{
    unsigned at = 0;
    for (at=0; at<nbytes && src[at]; ++at)
        dst[at] = src[at];
    char *retval = dst + at;
    for (/*void*/; at<nbytes; ++at)
        dst[at] = '\0';
    return retval;
}

char *
strcat(char *dst, const char *src)
{
    char *retval = dst;
    while (*dst)
        ++dst;
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
    return retval;
}

char *
strncat(char *dst, const char *src, unsigned nbytes)
{
    char *retval = dst;
    unsigned at;
    while (*dst)
        ++dst;
    for (at=0; at<nbytes && src[at]; ++at)
        *dst++ = src[at];
    *dst = '\0';
    return retval;
}

void *
__ctype_b_loc()
{
    return (void*)0x91100000; // arbitrary
}
