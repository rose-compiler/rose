#include "stdio.h"

#define goto1(i) \
goto c##i

int main(){    
    c1 : printf(" num is 1 \n");
    c2 : printf(" num is 2 \n");
    c3 : printf(" num is 3 \n");
    c4 : printf(" num is 4 \n");
    c5 : printf(" num is 5 \n");

    int i=4;

 // This is just a regular goto, not a computed goto.
    goto1(3);
}
