class A
{
public:
    int val;
    float fval;
public:
    A() { }
    void foo() { }
    int bar() { return 0; }
};

typedef int (A::*f_ptr)();

int main()
{
    int A::*aval_p = &A::val;
    f_ptr p_bar = &A::bar;
    A a, *ap;
    int *p = &a.val;
    a.*aval_p = 4;
    ap = &a;
    ap->*aval_p = 5;
    (a.*p_bar)();
    (ap->*p_bar)();
    return a.val;
}
