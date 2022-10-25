struct A
{
    int a;
    float b;
    A* next;
};

void bar(A&& rval_ref);

int foo1()
{
    bar(A());
    A&& a1 = A();
    bar(a1);
    A& a2 = *(new A());
    return 0;
}
