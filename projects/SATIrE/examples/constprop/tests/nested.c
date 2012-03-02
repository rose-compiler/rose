int f(int fn)
{
    return fn * fn;
}

int g(int gn)
{
    return gn * 2;
}

int h(int hn)
{
    return hn + 2;
}

int main()
{
    return f(g(h(0)));
}
