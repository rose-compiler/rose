
class A {

    // uninitialized member
    double member;

    public:
        A() {
            double var;

            // read of uninitialized member
            var = member;
        }
};


int main() {

    // error is in constructor
    A a;

    return 0;
}
