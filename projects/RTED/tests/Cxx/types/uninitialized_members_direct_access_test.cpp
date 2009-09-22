
class A {
    public: int x;

    A() {}
};


int main() {
    A a;
    // illegal read, x is not initialized
    int y = a.x;

    return 0;
}
