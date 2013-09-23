void foo(int& ref)
{
    int *p = &ref;
    *p = 10;
}

int main()
{
    int x = 1,y = 2;
    foo(x); 
    foo(y);
    return x+y;
}
