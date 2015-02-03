struct A;

struct A
{
    int val;
    A& next;
    A() : next(*this) { }
    A(A& _next) : next(_next) { }
};

void foo()
{
    A last;
    A list(last);
    list.next.val = list.val;
}
