void bar()
{
    int r, *q;
    q = &r;
}

void foo()
{
    int arr[10], x, y, *p;
    if(true)
        p = &x;
    else
        p = &y;
    arr[*p] = x + y;
}
