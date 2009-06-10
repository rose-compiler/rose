int sum(int *base, int n)
{
    int *p = base;
    int *end = base + n;

    int s = 0;

    while (p < end)
        s += *p++;

    return s;
}

void run(void)
{
    int arr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    sum(arr, sizeof arr / sizeof *arr);
}
