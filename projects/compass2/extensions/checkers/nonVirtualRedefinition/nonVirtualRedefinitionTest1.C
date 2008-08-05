class A {
public:
    virtual void mustOverride(int, float) = 0;
    virtual void canOverride(int);
    void doNotOverride(float *);
};

class B: public A {
public:
    void canOverride(int); // OK, base is virtual
};

class C: public B {
public:
    void mustOverride(int, float); // OK, base is virtual
    void canOverride(int); // OK, base is (implicitly!) virtual
    void doNotOverride(float *); // not OK, overriding nonvirtual function
};

class D: public A {
public:
    void doNotOverride(char *); // OK, not same function
};


// this definition is not reported again
void C::doNotOverride(float *)
{
}

namespace NonCompliant {
    class Base {
    public:
        virtual void overrideIfYouWish(int);
        void doNotOverride(int);
    };

    class Inherited: public Base {
    public:
        void doNotOverride(int); // trying to override nonvirtual function
    };
}

namespace Compliant {
    class Base {
    public:
        virtual void overrideIfYouWish(int);
        void doNotOverride(int);
    };

    class Inherited: public Base {
    public:
        void overrideIfYouWish(int); // overriding virtual function
    };
}
