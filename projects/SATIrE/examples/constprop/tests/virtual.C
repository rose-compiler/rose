class A
{
public:
    virtual int f(int n) { return n; }
};

class B : public A
{
public:
    int f(int n) { return 2 * n; }
};

void m(void)
{
    A *a = new A();
    a->f(3);
}
