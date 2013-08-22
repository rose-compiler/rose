void foo()
{
    int *p, arr[10];
    int i;
    p = arr;
    *(p + (i = 4)) = 12;
    *(p + (i = 4)) = arr[arr[i = 4]];
    arr[i=4] = 19;
}
