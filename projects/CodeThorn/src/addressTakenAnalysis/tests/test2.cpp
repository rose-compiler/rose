int q = 6;

int foo(int** p)
{
    *p = &q;
    return **p;
}

int main()
{
    int x, *p;
    x = 5;
    p = &x;
    int val = *p + foo(&p) + *p;
    return val;
}
