
class A {
    public:
        int* foo() { return new int; }
};


int main() {
    if( true ) {
        A* a = new A;
        int* x = a -> foo();

        delete a;
    // memory leak on scope exit
    }

}
