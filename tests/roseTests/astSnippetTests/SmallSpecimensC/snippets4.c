#include "snippetsRelated.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The rest of the file is snippets
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void notNull(const void *x) 
{
    assert(x != (const void*)0);
}

void checkedCopy(void *dst, const void *src, unsigned nbytes)
{
    notNull(dst);
    notNull(src);
    if (dst != src)
        memcpy(dst, src, nbytes);
}

void storeHeapString(const char *from)
{
    char *storage;
    unsigned tmp_nbytes;

    tmp_nbytes = strlen(from) + 1;
    storage = malloc(tmp_nbytes);
    checkedCopy(storage, from, tmp_nbytes);
}

void loadHeapString(char *to, char *from)
{
    unsigned tmp_nbytes;

    tmp_nbytes = strlen(from) + 1;
    checkedCopy(to, from, tmp_nbytes);
    free(from);
}

void storeStructString(const char *from)
{
    struct Struct1 storage;
    storage.const_string_member = from;
}

void loadStructString(const char *to, struct Struct1 from)
{
    to = from.const_string_member;
}
