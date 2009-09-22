
class A {
    public: int x;

    A() {}

    void foo() {
        // illegal read, x is not initialized
        int y = x;
    }
};


int main() {
    A a;
    a.foo();

    return 0;
}
