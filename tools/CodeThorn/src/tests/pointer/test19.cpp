void foo()
{
    int arr[10], arrB[10], a;
    int *a_ptr1, *a_ptr2, *x, *y, (*arr_ptr)[10];
    x = arr; y = arrB;
    (a_ptr1 = arr)[2] = 2;
    (a_ptr2 = x + 4)[2] = 4;
    (true? a_ptr1 = arr : a_ptr1 = arrB)[2] = 2;
    (true? a_ptr1 = arr + 10 : a_ptr2 = arrB + 10)[2] = 2;
    (true? a_ptr1 = x++ : a_ptr1 = x+sizeof(arr))[2] = 0;
    (true? x : y) = 0;
    a_ptr1 = &((true? a_ptr1 = arr : a_ptr1 = arrB)[2]);
    arr_ptr = &(a_ptr2? arr : arrB);
} 
