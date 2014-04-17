int foo1()
{
    int *p, **q;
    int vals  = 5;
    p = &vals;
    q = &p;
    vals = *p + **q;
    return vals;
}
