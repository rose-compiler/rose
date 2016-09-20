// using 'class' instead of 'typename' for template-class typedefs

// [dunno if this is a gcc-bug false positive -- Karl]

// originally found in package

// a.ii:13:9: error: there is no variable called `var1'
// a.ii:14:9: error: there is no variable called `var2'
// a.ii:18:15: error: `V<int>::foo_t' is a typedef-name, so cannot be used after 'struct' (inst from a.ii:25:13)
// a.ii:19:14: error: `V<int>::foo_t' is a typedef-name, so cannot be used after 'class' (inst from a.ii:25:13)
// a.ii:13:9: error: there is no variable called `var1' (inst from a.ii:26:5)
// a.ii:14:9: error: there is no variable called `var2' (inst from a.ii:26:5)

// ERR-MATCH: there is no variable called `.*?'

struct A {};

template<class T> struct V {
    typedef A foo_t;
};

template<class T> struct S2 {
    int foo() {
        //ERROR(1): var1;
        //ERROR(2): var2;
        var3;
        return 42;
    }
    //ERROR(1): struct V<T>::foo_t var1;
    //ERROR(2): class V<T>::foo_t var2;
    typename V<T>::foo_t var3;
};

int main()
{
    S2<int> s2;
    return s2.foo();
}

