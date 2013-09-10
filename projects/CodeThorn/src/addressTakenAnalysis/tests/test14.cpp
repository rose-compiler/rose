void foo()
{
    int arr[10];
    int *p, **q;
    q = &p;
    p = &arr[4];
    q = &(*q);
}
