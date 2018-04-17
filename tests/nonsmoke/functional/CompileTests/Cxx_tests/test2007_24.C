// Test name collision within scopes

// This makes for a wonderful DOT graph showing that the types of "Y1" and "Y2" are correct.

// typedef using the same name for a class defined in the typedef as for the typedef type
// (symbols for both types are placed into global scope using the same name "X").
typedef struct X {} X;

// This variable declaration's type is the typedef type "X"
X Y1;

// This variable declaration's type is the class type "X"
struct X Y2;
