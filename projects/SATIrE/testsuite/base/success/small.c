void f(void)
{
    int x, y, z;

    x = 7;
there:
    if (y)
    {
        z = 0;
    }
    else
        z = 2;
    y = z;
        goto there;
}

void g(void)
{
    int x = 3;
}
