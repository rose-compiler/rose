
class Type {

    class Nested {
        int a;
        int b;
    } var;
};


int main() {

    int x;
    int* p;

    // fairly straightforward segfault.  The point of this test is to verify
    // that we can actually transform the nested class construct
    x = *p;

    return 0;
}
