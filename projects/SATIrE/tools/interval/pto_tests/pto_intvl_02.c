void get(int what, int *p)
{
    if (what == 1)
        *p = 42;
    else
        *p = 0;
}

void use()
{
    int n, m;
    get(0, &n);
    get(1, &m);
}
