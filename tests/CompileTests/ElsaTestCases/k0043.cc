// voiding the result of a member call

// originally found in package amule_1.2.6+rc7-2

// c.ii:9:12: Parse error (state 676) at .

// ERR-MATCH: Parse error.*at ([.]|->)$

struct S1 {
    int foo() { return 0; }
} s1, *ps2;

int main()
{
    void(s1.foo());
    void(ps2->foo());

    return 0;
}
