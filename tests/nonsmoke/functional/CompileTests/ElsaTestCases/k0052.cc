// operator& returning other pointer type

// originally found in package tetex-bin_2.0.2-25

// a.ii:13:5: error: cannot convert argument type `struct S2 *' to parameter 1 type `struct S1 *'

// ERR-MATCH: cannot convert argument type `.*?[*]' to .*? type `.*?[*]'

struct S1 {};

int foo(S1* obj) { return 42; }

struct S2 {
    S1* operator&() { return 0; }
};

int main() {
    S2 s2;
    return foo(&s2);
}
