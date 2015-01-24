struct B { 
    int _b;

    B* operator->() {
        return this;
    }
    B* operator=(const B& other) {
        _b = other._b;
        return this;
    }

    void foo() { }
};

int main()
{
    B b, c;
    b->foo(); b.foo();
    b = c;
} 
