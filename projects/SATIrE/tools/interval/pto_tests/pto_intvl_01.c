int server(int *p)
{
    int sum = 0;
    int i = 1;
    int limit = *p;
    while (i <= limit)
    {
        sum = sum + i;
        i++;
    }
    return sum;
}

int client()
{
    int n = 23;
    int m = 42;

    int s1 = server(&n);
    int s2 = server(&m);

    return s1 + s2;
}
