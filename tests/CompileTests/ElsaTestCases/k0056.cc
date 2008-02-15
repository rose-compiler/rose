// template friend function

// originally found in package arts

// Assertion failed: !destVar->funcDefn, file template.cc line 3073

// ERR-MATCH: Assertion failed: !destVar->funcDefn

template <typename T>
struct S1 {
    template <typename T1> friend int foo();
};

template <typename T1> int foo() {
    return 42;
}

int main()
{
    S1<long> s1;
}
