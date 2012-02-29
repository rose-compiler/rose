namespace A
{
    int x = 42;
    int f();
}

namespace B
{
    int x = 23;
    int f();
}

int A::f()
{
    return x;
}

int B::f()
{
    return x;
}

int f()
{
    int n = 7;
    return A::f() + B::f() + n;
}

void init_globals()
{
    A::x = 42;
    B::x = 23;

    f();
}
