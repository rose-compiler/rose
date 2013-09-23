struct A
{
    int a;
    float b;
    A() : a(0), b(0.0) { }
    A(int _a, float _b) : a(_a), b(_b) { }
};

int gvar = 4.0;

struct A a(4,0.1);

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
