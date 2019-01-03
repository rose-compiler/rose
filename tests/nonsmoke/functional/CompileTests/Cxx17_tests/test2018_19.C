// Modernizing using-declarations

namespace A {
    int i;
}

namespace A1 {
    using A::i;
    using A::i, A::i; // OK: double declaration
}
    
struct B {
    int i;
};

struct X : B {
    using B::i;
 // using B::i, B::i; // error: double member declaration
};
