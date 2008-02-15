// 'and' etc. not keywords in C

// originally found in package

// In state 86, I expected one of these tokens:
//   <name>, auto, bool, char, const, double, extern, float, friend, inline, int, long, mutable, operator, register, short, signed, static, template, typedef, unsigned, virtual, void, volatile, wchar_t, (, ), [, ::, ~, &, *, >, ,, ;, __attribute__, restrict,
// a.i:4:9: Parse error (state 86) at !

// ERR-MATCH:

int main() {
    int not;
    int and;
    return 0;
}
