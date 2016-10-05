// template friend function (2)

// originally found in package arts

// Assertion failed: ct->parameterizingScope == this, file cc_scope.cc line 53

// ERR-MATCH: Assertion failed: ct->parameterizingScope == this

template <typename T>
struct S1 {
    template <typename T1> friend int foo();
};

template <typename T1> int foo() {
    return 42;
}

template <typename T>
struct S2 {
    S1<long> s1;
};

int main()
{
    S2<int> x;
    return foo<int>();
}
