struct A;

struct A
{
    int val;
    A() { }
};

void bar(A a)
{
    a.val = (int)'b';
}

void foo()
{
    A a, a_next;
    a_next.val = a.val;
    a_next = a;
    bar(a_next);
}
