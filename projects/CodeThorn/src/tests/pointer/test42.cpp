struct A
{
    int **p;
};

typedef struct A sA;
typedef int MYINT;
typedef int MYARR[10];

int foo()
{
    int **p;
    MYINT m_arr[10][10];
    MYARR my_arr[10];
    int* arr_p[10];
    sA sa;
    int arr[10][10];
    p[0] = arr[0];
    arr_p[0] = arr[0];
    p[1][0] = 2;
    sa.p[0] = arr[0];
    arr_p[0][3] = 3;
    (sa.p)[1][2] = 0;
    m_arr[0][2] = 4;
    my_arr[0][0] = 5;
    return arr[1][0] + arr[1][2];
}
