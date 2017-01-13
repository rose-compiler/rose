// there is no action to merge nonterm MemberDeclaration

// originally found in package tcl8.4

// ERR-MATCH: merge nonterm MemberDeclaration

typedef void (*func) ();
struct S {
    func (*proc);
};
