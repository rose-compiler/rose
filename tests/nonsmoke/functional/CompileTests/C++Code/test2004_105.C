
struct Type { int x; };

// The first part of the type is: "struct tag { int x; }"
// And there is no 2nd part!
struct tag { int x; } Var;

// mixing unnamed types in typedefs with arrays
typedef struct { int x; } ArrayType [100];

// The first part of the type is: "struct { int x; }"
// and the second part is: "[100]"
// but we have to be careful to extract the base type from the SgArrayType
struct { int x; } ArrayVar [100];
