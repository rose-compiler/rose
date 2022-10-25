void foo(int &x_ref)
{
    int *p = &x_ref;
    *p = 10;
}

void bar(int &y_ref)
{
    int *q = &y_ref;
    *q = 20;
}

int main()
{
    int x=2, y=1;
    void (*f_ptr)(int&);
    f_ptr = foo;
    if(x > y) f_ptr = foo;
    else      f_ptr = bar;
    f_ptr(x);
    return x+y;
}
