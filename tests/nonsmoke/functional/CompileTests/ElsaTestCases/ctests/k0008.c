// function call vs type-cast ambiguity

// this happens with 5 nested func calls, but not 4 or less.

// originally found in package emacs21_21.3+1-8

// a.i:10:25: error: variable name `func' used as if it were a type
// a.i:10:13: error: variable name `func' used as if it were a type
// a.i:10:19: error: variable name `func' used as if it were a type
// Assertion failed: unexpected ASTTypeId ambiguity, file cc_tcheck.cc line 864

// ERR-MATCH: as if it were a type

int func(int x) {
    return x;
}

int main()
{
    int foo;
    return (func (func (func (func (foo))))) + 0;
}
