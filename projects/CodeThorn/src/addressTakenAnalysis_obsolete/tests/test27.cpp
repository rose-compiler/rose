int main()
{
    int x = 1,y = 2;
    int &x_ref = x;
    int &y_ref = y;
    int *p, *q;

    p = &x_ref; q = &y_ref;
    *p = 5;
    *q = 10;
    return *p + *q;
}
