class A
{
public:
    A() {}
    virtual ~A() {}
};

class B : public A
{
public:
    B() {}
    ~B() {}
};

int main()
{
    A *a = new A();
    B *b = new B();
    A c;
    delete a;
    delete b;
    return 0;
}
