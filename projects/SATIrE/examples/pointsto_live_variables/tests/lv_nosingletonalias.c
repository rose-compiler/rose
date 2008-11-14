int fac()
{
    int n;
    int result;
    int *np;
    int not_used;

    np = &n;
    np = &not_used;

 // cannot kill *np here because it does not refer to a singleton location
    *np = 42;

    result = 1;
    while (*np < 0)
    {
        result = result * *np;
        (*np)--;
    }
}
