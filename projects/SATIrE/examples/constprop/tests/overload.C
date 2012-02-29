int f(long n)
{
    return n;
}

int f(long u, long v)
{
    return u + v;
}

int f(long u, long v, long w)
{
    return u * (v + w);
}

void g()
{
    int x = 7;
    int a = f(x);
    int b = f(x, a);
    int c = f(x, a, b);
}
