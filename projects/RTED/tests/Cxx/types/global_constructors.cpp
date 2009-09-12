
class A {
    public:
        int* member;

        A() {
            // A needs to have been registered before this point
        }
        ~A() { delete member; };
};

// constructor, pre-main function call
A a;


int main() {

    // illegal read
    int x = *(a.member);

    return 0;
}
