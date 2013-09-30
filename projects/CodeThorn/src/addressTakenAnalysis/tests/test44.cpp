void foo(int (&arr)[10][10])
{
    arr[0][0] = 1;
}

void bar()
{
    int arr[10][10];
    foo(arr);
}
