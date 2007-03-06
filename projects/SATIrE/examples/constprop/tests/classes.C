class A
{
public:
    virtual int f() const { return 42; }
    static int c() { return 0; }
};

class B
{
public:
    int f() const { return 23; }
};

class C : public A
{
public:
    virtual int f() const { return 1; }
};

int f()
{
    A *a;
    B *b;
    C *c;

    return a->f() + b->f();
}
