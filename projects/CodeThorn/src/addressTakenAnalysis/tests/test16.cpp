struct A;

struct A
{
    int val;
    A() { }
};

void foo()
{
    A a, a_next;
    a_next.val = a.val;
}
