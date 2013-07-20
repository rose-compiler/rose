struct A
{
    int a;
    float b;
};

int foo1()
{
    int *p, **q;
    A sA, *sAp;
    int vals  = 5;
    float f, *fp;
    f = 4.0;
    fp = &f;
    p = &sA.a;
    q = &p;
    sAp = &sA;
    return 0;
}
