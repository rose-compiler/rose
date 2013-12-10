/* Specimen for testing snippet injection */
#include <assert.h>
int INSERT_HERE;

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
