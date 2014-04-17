struct A;

struct A
{
    int val;
    A() { }
    A(int _val) : val(_val) { }
};

void foo()
{
    A a, a_next;
    A a1(4);
    a_next.val = a.val;
    a_next = a;
}
