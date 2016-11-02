// 13.2a.cc

class B {
public:
    int f(int);
};

class D : public B {
public:
    int f(char*);
};

// Here D::f(char*) hides B::f(int) rather than overloading it.

void h(D *pd)
{
    //ERROR(1): pd->f(1);               // error:
                                        // D::f(char*) hides B::f(int)
    pd->B::f(1);                        // OK
    pd->f("Ben");                       // OK, calls D::f
}


