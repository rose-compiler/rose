#include <stdio.h>

struct T
{
    int x;
    char buf[10];
};

void test(struct T *P)
{
    struct T *A;
    int *pInt;
    char *pBuf, *p0, *p5;
    A = &P[0];
    pInt = &(P[0].x);
    p0 =  & (A->buf[0]) ;
    p5 =  & (A->buf[5]) ;
/*    *pInt = *pInt + 1;
    *p0 = 'c';
    *p5 = 'd'; */
}

/*int main(int argc, char *argv[])
{
    struct T type;
    type.x = 10;
    type.buf[0] = 'a';
    type.buf[5] = 'b';
    printf("%d, %c, %c\n", type.x, type.buf[0], type.buf[5]);
    test(&type);
    printf("%d, %c, %c\n", type.x, type.buf[0], type.buf[5]);
    return 0;
}*/
