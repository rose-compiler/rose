int q = 6;

int foo(int** p)
{
    *p = &q;
    return **p;
}

int main()
{
    int x, *p, *r;
    int **q;
    q = &p;
    x = 5;
    p = &x;
    r = 0;
    int val = *p + foo(&p) + *p;
    *(q + 2) = &val;
    *((p = &x) + 2) = 0;
    val = x + (*p = 5);
    *(int*)(p-r) = 5;
    // 
    //
    *p=1000;
    return val;
}
