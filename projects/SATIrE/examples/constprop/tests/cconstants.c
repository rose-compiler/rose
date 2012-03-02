int add(int, int);
int mul(int, int);
int zero(void)
{
    return 0;
}

int f(void)
{
    int x = add(1, zero());
    int y = 7;
    int z = mul(7, 3);

    if (x <= 2 || x > 23)
        z = x + z;
    else
        y = 42;

    return y + z;
}

int add(int a, int b)
{
    return a + b;
}

int mul(int a, int b)
{
    return a * b;
}
