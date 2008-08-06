class A
{
public:
    const A &operator=(const A &other) { foo = other.foo; return *this; }

private:
    int foo;
};

void foo(const A &);

void f(A &lhs, A &rhs)
{
    lhs = rhs; // OK: assignment discarded
    foo(rhs = lhs); // not OK: assignment not discarded
}

int count_f(const char *str)
{
    int n;
    n = 0; // OK: assignment discarded
    char c;
    while ((c = *str++) != '\0') // not OK: assignment not discarded
        if (c == 'f')
            n++;
    return n;
}
