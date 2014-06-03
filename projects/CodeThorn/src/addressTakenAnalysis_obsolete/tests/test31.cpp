struct A
{
    int p;
    A(int _p) : p(_p) { }
};

void foo()
{
    A  a(4);
    a.p = 5;
}
