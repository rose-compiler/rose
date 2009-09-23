
class A {
    public:
        int* foo() { return new int; }
};


int main() {
    if( true ) {
        A* a = new A;
        a -> foo();
        int* x = a -> foo();


    // memory leak on scope exit
    }

}
