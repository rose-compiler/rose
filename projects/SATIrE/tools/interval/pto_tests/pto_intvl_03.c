// more advanced version of pto_intvl_02.c

void get(int what, int *p)
{
    if (what == 1)
        *p = 42;
    else
        *p = 0;
}

void use()
{
    int n = 666, m, x, y;
    get(0, &n);
    get(1, &m);
    get(x, &y);
    y = 7;
    get(x, &y);
}
