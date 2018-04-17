// template super class member variable

// originally found in <vector>

// vx.ii:9:9: error: there is no variable called `var'

// ERR-MATCH: there is no variable

template<typename T>
struct B {
    int var;
};

template<typename T>
struct C : B<T> {
    C() {
        var = 0;
    }
};

int main()
{
    C<int> b;
    return 0;
}
