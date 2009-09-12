
class B;
class A {
    public:
        ~A() { if( member ) delete member; }

        B* member;
};

class B {
    public:
        B() {
            int* x;
            // null deref
            int y = *x;
        }
};


int main() {

    A a;
    // error is in B's constructor
    a.member = new B;

    return 0;
}
