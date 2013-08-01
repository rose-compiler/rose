class A
{
public:
    int i;
    static float& f;
};

int& bar(int *x)
{
    return *x;
}

void foo()
{
    int *p, *q;
    int a, b;
    int (* arr_p)[10];
    int arr[10][10];
    p = &(a+1, (a+1, b));
    p = &(arr[2][3]);
    arr_p = &(arr[2]);
    void (*f)() = &foo;
    q = &(*(new int[5]));
    int A::*i_ptr = &A::i;
    float* f_ptr = &A::f;
    q = &(bar(p));
    p = &(*p? a : b);
}
