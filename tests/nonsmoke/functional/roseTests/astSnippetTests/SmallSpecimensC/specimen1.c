static int INSERT_HERE;                 // a marker used by the injectSnippet test
#include "snippetsRelated.h"
#include <stdlib.h>
#include <string.h>

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
#pragma message "this is a pragma, which is a kind of SgDeclarationStatement"
}

double
ipoint4()
{
    INSERT_HERE;
}

int
main(int argc, char *argv[]) 
{
    return 0;
}
