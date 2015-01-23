void foo()
{
    int *p, *q, val, **r;
    p = &val;
    q = &(*p);
    r = &(*(&p));
}
