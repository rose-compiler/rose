int sum(int n)
{
    int i = 0;
    int s = 0;

    while (i <= n)
    {
        s = s + i;
        i++;
    }

    return s;
}

int run()
{
    int result;

    result = sum(42);

    return result;
}
