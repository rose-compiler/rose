
// struct X { int __val[2]; } myVariable;

#if 1
struct X { int __val; } myVariable;
#else
struct X { int __val; };
#endif
