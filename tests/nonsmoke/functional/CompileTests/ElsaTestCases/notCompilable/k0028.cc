// definition of member function declared in another namespace

// originally found in package kdelibs

// error: function definition of `S::foo' must appear in a namespace that
// encloses the original declaration

// sm: This is invalid C++.

// ERR-MATCH: must appear in a namespace

namespace NS1 {
    struct S {
        int foo();
    };
}

using namespace NS1;

namespace NS2 {
    int S::foo() { return 0; }
}



// ---- variant without the 'using' directive ----
namespace NS3 {
    struct S {
        int foo();
    };
}

namespace NS4 {
    int NS3::S::foo() { return 0; }
}
