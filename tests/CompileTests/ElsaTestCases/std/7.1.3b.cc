// duplicate typedefs

typedef struct s { /* ... */ } s;
typedef int I;
typedef int I;
typedef I I;

// this isn't part of the official example, but it lets
// me verify that "I" maps to "int"
I x;

