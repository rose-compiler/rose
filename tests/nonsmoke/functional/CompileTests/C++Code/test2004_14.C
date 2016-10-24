// Another bug report from Chadd

/*****START CODE*****/
#include <stdarg.h>
#include <stdio.h>

void va_function(int *r, ...)
{
        va_list va;
        int *s;

        va_start(va, r); /* error for ROSE*/
        printf("ARG: %d\n", *r);
        while (1) {
                s = va_arg(va, int *); 
                printf(" ARG: %d\n", *s);
                if (*s == 3)
                    break;

        }
        va_end(va);
}


int main(){

        int *x,*y,*z;
        int realX=1, realY=2, realZ=3;
        x = &realX;
        y = &realY;
        z = &realZ;
        va_function(x,y,z);
}
/*****END CODE*******/
