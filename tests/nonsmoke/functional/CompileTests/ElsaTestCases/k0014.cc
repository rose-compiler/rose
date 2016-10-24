// ambiguous lookup of base class constructor with constructor of same class
// with different template argument

// Assertion failed: !ambiguity, file cc.ast.gen.cc line 2726

// originally found in package aspell

struct B {};

template <typename T> struct C : B {
    C() {}
    C(C<int> & other) : B(other)
    {
        foo();
    }

    void foo() {}
};


int main() {
    C<int> c1;
    C<long> c2(c1);
}
