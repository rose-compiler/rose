// Tests for variable renaming. Any variable whose name begins with "tmp" is renamed
// so it doesn't conflict with other variables that might be visible at the insertion point.
#include <stdlib.h>                                     // random
#include <string.h>                                     // strlen

void randomOffByOne(int arg1)
{
    int tmp_offset = random() % 3 - 1;
    arg1 += tmp_offset;
}

// Verify that copies of some things can be suppressed. See the doNotInsert calls in injectSnippet.C
void shouldNotBeInserted() {}
typedef int SomeOtherInteger;
SomeOtherInteger someOtherInteger;
struct SomeOtherStruct {
    int dummy;
};

// Verify that some miscellaneous things are copied into the target.
typedef int SomeInteger;
SomeInteger thisIsAnInteger;
struct SomeStruct {
    int dummy;
};

void addWithError(int addend1, int addend2, int result)
{
    typedef int typeof_result;
    randomOffByOne(addend1);
    randomOffByOne(addend2);
    result = addend1 + addend2;
}

void copy_string10(char *dst, const char *src)
{
    unsigned tmp_size = strlen(src);
    tmp_size = tmp_size > 10 ? 10 : tmp_size;
    memcpy(dst, src, tmp_size);
    dst[tmp_size] = '\0';
}

void allocate_string(const char *s) 
{
    unsigned tmp_i;
    unsigned tmp_size = strlen(s) + 1;
    char *heap_storage = malloc(tmp_size);
    copy_string10(heap_storage, s);
    for (tmp_i=10+1;  tmp_i<tmp_size; ++tmp_i)
        heap_storage[tmp_i] = random() % 2 ? ' ' : '\t';
}
