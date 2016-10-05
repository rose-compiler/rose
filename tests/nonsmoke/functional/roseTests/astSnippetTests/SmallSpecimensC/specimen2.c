/* Specimen for testing snippet injection */
#include <assert.h>
int INSERT_HERE;

#include "snippetsRelated.h"

#include <string.h>
#include <stdlib.h>                                     // free
#include <string.h>                                     // memcpy

// Function declarations required from snippet storeHeapString
void checkedCopy(void *dst, const void *src, unsigned nbytes);
// char* malloc(unsigned tmp_nbytes);
// unsigned strlen(char* from);
// size_t strlen(const char *s);
void notNull(const void *x);

// Function declarations required from snippet loadHeapString
// void free(char* from);

// Function declarations required from snippet ?
// void assert(bool);

int main()
{
    const char *s, *t;
    int digest = 0;

    s = "Hello, world!";
    INSERT_HERE;

    for (t=s; *t; ++t)
        digest += *t;
    return digest % 128;
}
