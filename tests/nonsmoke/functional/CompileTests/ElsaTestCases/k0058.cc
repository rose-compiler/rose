// converting to types from parameter templates

// originally found in <string>

// a.ii:17:9: error: cannot convert argument type `int' to parameter 1 type `A::A_int'
// a.ii:17:9: error: cannot convert argument type `int' to parameter 1 type `A::A_int'
// a.ii:20:9: error: cannot convert argument type `int &' to parameter 1 type `A::A_int'

// ERR-MATCH: cannot convert argument type `int' to parameter 1 type `.*?::.*?'

struct SA {
    typedef int A_int;
};

template<typename A>
struct S1 {
    typedef typename A::A_int my_int;
    int func0(typename A::A_int p) {
        return p;
    }
    int func1(my_int p) {
        return p;
    }
    int func2() {
        return func0(6) * func1(7);
    }
    int func3(int i) {
        return func0(i);
    }
};

int main() {
    S1<SA> s1;
    s1.func3(3);
    return s1.func2();
}
