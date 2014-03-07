static int INSERT_HERE;                 // a marker used by the injectSnippet test
#include "snippetsRelated.h"
#include <stdlib.h>
#include <string.h>

// DQ (3/2/2014): This is required to be present under the new rules for lookup of symbols referenced by the snippet.
#include <stdio.h>                                      // fputs

// Required function declaration for test5a
void randomOffByOne(int addend1);

// Required function declaration for test5c
void addWithError(int addend1, int addend2, int result);

// Required function for test 6a
void copy_string10(char *dst, const char *src);

// Required function for test 6a
void allocate_string(const char *s);

int
ipoint1()
{
    int x = 1;
    int y = 2;
    int z = 3;
    INSERT_HERE;
    return x;
}

char *
ipoint2()
{
    char *s1, *s2, *s3;
    s1 = strdup("Hello, world!");
    s2 = strdup("Goodbye, cruel world.\n");
    s3 = malloc(strlen(s1) + strlen(s2) + 1);

    INSERT_HERE;
    strcpy(s3, s1);
    strcat(s3, s2);
    return s3;
}

double
ipoint3()
{
    double f1 = 3.14;
    double f2 = 299792458.0;
    double f3 = f1 + f2;

    INSERT_HERE;
    return f3;
}


int
main(int argc, char *argv[]) 
{
    return 0;
}
