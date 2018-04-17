// old-style function decl with parentheses around func name

// originally found in package bash

// error: Parse error (state 264) at int

// ERR-MATCH: state 264

int (foo) (c)
    int c;
{
}
