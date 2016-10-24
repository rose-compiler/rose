// re-declaration of template base-class member

// originally found in package jade_1.2.1-43

// a.ii:10:8: error: class `A' isn't a template (inst from a.ii:14:12)

// ERR-MATCH: class `.*?' isn't a template

template<class T> struct A {
protected:
    int foo() { return 42; }
};

template<class T> struct B : A<T> {
public:
    A<T>::foo;
};

int main() {
    B<int> b;
    return b.foo();
}
