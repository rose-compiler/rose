int f(void)
{
    int n = 0;

    while (n < 100)
    {
        int while_var;
        if (n % 3 == 0)
        {
            int while_if_var;
            n += 17;
            return n;
        }
        n += 16;
    }
    return n;
}
