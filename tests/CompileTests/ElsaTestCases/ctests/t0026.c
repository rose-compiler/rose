// t0026.c
// decls differ by cv-qualification of return type

// Though both GCC and ICC accept this, I don't know why.

// I think it should be illegal because:
//   - 6.7p4 says declarations of same entity must use 
//     compatible types
//   - 6.7.5.3p15 says compatible function types require
//     compatible return types
//   - 6.7.3p9 says that for two types to be compatible,
//     the qualifiers must be identical

// Oh well, Elsa will accept it.

typedef volatile int intfn();

intfn foo;

int foo(int x)
{}
