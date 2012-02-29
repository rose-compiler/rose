void get(int what, int *p)
{
    if (what == 1)
        *p = 42;
    else
        *p = 0;
}

void run()
{
    int a, b, c;
    int *pa = &a;
    int *pbc;
    pbc = &b;
    pbc = &c;

    a = 23;
    b = 100;
    c = 200;

    get(0, &a);
    get(1, &b);
    get(2, &c);
}
