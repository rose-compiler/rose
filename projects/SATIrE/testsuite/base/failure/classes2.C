class A
{
    int n;

public:
    A() : n(13) {}
    A(int x) : n(x) {}
    virtual int f(int);
};

class B : public A
{
public:
    int f(int);
};

class C : public A
{
    int drei, vier;

public:
    int f(int);
    C() : drei(3), vier(4), A(7) {}
};

int B::f(int n)
{
    return 22 + n;
}

int C::f(int n)
{
    return 42 - n;
}

int f()
{
#if 0
    A *a = new A(42);
    return a->f(10);
#else
    A a(42);
    return a.f(10);
#endif
}
