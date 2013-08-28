struct A
{
    int type;
    int blah;
};

void foo()
{
    A arr[10], arrB[10], a;
    A *a_ptr1, *a_ptr2, *x, *y;
    x = arr; y = arrB;
    (a_ptr1 = arr)[2].type = 2;
    (a_ptr2 = x + 4)[2].type = 4;
    (true? a_ptr1 = arr : a_ptr1 = arrB)[2].type = 2;
    (true? a_ptr1 = arr + 10 : a_ptr2 = arrB + 10)[2].type = 2;
    (true? a_ptr1 = x++ : a_ptr1 = x+sizeof(arr))[2].type = 0;
    (true? x : y) = 0;
} 
