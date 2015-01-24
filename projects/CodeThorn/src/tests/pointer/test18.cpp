struct A { void foo(); };
struct B { A* operator->(); };

int main()
{
    B b;
    b->foo();
} 
