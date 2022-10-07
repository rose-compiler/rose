struct A
{
    int a;
    float b;
};

int foo1()
{
    int *p, **q;
    A sA;
    int vals  = 5;
    float f, *fp;
    f = 4.0;
    fp = &f;
    p = &sA.a;
    q = &p;
    return *p;
}
