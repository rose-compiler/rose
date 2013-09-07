struct A {
    void foo() {
    }
};

struct B { 
    int _b;
    A* a_ptr;
    B() : _b(0), a_ptr(0) { }
    B(A* _ap) : a_ptr(_ap) { }

    A* operator->() {
        return a_ptr;
    }
    void foo() { 
    }
};

struct C {
    int _c;
    void foo() { }
    C operator=(const C& other) {
        C r_c = {other._c};
        return r_c;
    }
};

int main()
{
    A a;
    B b, *b_p;
    b_p = &b;
    b->foo();
    b_p->foo();
    C c1={4}, c2;
    c2 = c1;    
} 
