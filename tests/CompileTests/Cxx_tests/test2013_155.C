// The first part of the type is: "struct { int x; }"
// and the second part is: "[100]"
// but we have to be careful to extract the base type from the SgArrayType
// struct { int x; } ArrayVar [100];

// DQ (3/10/2007): I think this make this example more complex in a relavant way
struct { int x; } anotherArrayVar [100],struct_abc;

