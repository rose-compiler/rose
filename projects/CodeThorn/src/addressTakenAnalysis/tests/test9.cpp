void foo()
{
    int arr[10];
    int arr2[10];
    int *p = arr;
    int *q = &arr[4];
    *(arr + *p) = 5;
    *(arr[2] + arr) = 0;
}
