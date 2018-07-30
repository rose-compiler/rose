#include <iostream>
#include <cstdlib>

class A {
    public: 
    int a;
    A() { std::cout << "A constructor called\n"; a = 2; }
    A(const A &other) {
        std::cout << "A copy constructor called\n";
        a = 5;
    }
    ~A() { }
};

class B : public A {
    public:
    int b;
    B() { std::cout << "B constructor called\n"; b = 4; }
    B(const B &other) {
        std::cout << "B copy constructor called\n";
    }
    ~B() { }
};

int main() {
    class B b_obj;
    printf("a: %d\n", ((class A)b_obj).a);
    return 0;
}
