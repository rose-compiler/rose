// Test for allignment attribute (reported to be missing in output code, 2/6/2014)

#include <stdio.h>

int main() {
    int x __attribute__((aligned(64)));
    fprintf (stderr, "alignment: %zu\n", ((size_t) &x % 64));

    return 0;
}

