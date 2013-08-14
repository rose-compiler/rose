int main()
{
    int arr[10][10];
    int (*p_arr)[10];
    int i, j;
    i = j = 1;
    p_arr = arr;
    *(*(arr + i) + (j = 2)) = 10;
    return arr[1][1];
}
