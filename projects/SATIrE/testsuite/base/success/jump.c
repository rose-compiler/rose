void f(void)
{
    int x = 7;

    while (x > 3)
    {
        x--;
        if (x == 7)
            goto here;
    }
here:
    x = 17;
}

void g(void)
{
    int x;

    if (x == 23)
        goto here;
    else
        goto here;
    // x = 5; // unreachable
here:
    x = 42;
}
