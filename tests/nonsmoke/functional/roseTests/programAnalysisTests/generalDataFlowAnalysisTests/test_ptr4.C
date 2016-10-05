int a;


void main()
{
    int **p;
    int *x;

    a = 20;

    x = &a;
    #pragma rose a: Aliases:{ }{}\
    p: Aliases:{ }{}\
    x: Aliases:{ a }{}\
    b: Aliases:{ }{}

    p = &x;
    #pragma rose a: Aliases:{ }{}\
    p: Aliases:{ x }{}\
    x: Aliases:{ a }{}\
    b: Aliases:{ }{}

    int b=0;
    *p = &b;
    #pragma rose a: Aliases:{ }{}\
    p: Aliases:{ x }{}\
    x: Aliases:{ b }{}\
    b: Aliases:{ }{}

    int **q = p;
}
