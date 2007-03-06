
#include <stdio.h>
#include <ctype.h>

typedef struct {
    int x;
} NODE;

int xlpeek(NODE *fptr)
{
    return 1;
}

int xlgetc(NODE *fptr)
{
    return  0;
}

int main (int argc, char** argv)
{
    int ch;
    NODE* fptr;

    while ((ch = xlpeek(fptr)) != EOF && isspace(ch))
        xlgetc(fptr);

    return ch;
}
