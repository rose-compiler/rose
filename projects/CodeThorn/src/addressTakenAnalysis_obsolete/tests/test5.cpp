struct A
{
    int a;
    float b;
    A* next;
};

int foo1()
{

    A sA, *sAp;
    sA.a = 5;
    sA.b = 4.0;
    sAp = 0;
    sA.next = sAp;
    ((sA.next)+1)->next = sAp;
    return 0;
}
