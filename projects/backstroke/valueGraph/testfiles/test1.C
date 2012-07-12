void foo(int&a, int&b, int&c, int&d)
{
    if (a > 0)
    {
        a = 0;
        if (b > 0)
            goto L;
        d = 0;
    }
    else
    {
        b = 0;
L:
        if  (c > 0)
        c = 0;
    }
}

