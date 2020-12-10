struct A {};

// Unparsed as: struct B  (missing base class ": A"), but only in variable declaration.
struct B : A {};

