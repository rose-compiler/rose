int fac()
{
    int n;
    int result;
    int *np = &n;

 // Ideally, this should kill n (which it does) and not generate n, so n
 // should be dead before this assignment. However, the current definition
 // of gen is not quite smart enough, so we get a little imprecision.
    *np = 42;

    result = 1;
    while (*np < 0)
    {
        result = result * *np;
        (*np)--;
    }
}
