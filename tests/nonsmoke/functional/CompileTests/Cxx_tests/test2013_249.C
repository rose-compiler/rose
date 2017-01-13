#if 0
Hi Dan
 Here is a test code that we found where the unparser did not emit correct code.
------
struct A { void foo(); };
struct B { A* operator->(); };
struct C { B operator->(); };
struct D { C operator->(); };
int main()
{
    D d;
    d->foo();
} 
-------
The unparser generated the following expression for d->foo() 

d ->  ->  ->  foo ();

Thanks
Sriram

PS: Milind suspects that this is the reason why AST normalization check in tests are failing
#endif

struct A { void foo(); };
struct B { A* operator->(); };
struct C { B operator->(); };
struct D { C operator->(); };
int main()
{
    D d;
    d->foo();
} 
