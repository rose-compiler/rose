// Declaring non-type template parameters with auto

template<const int* pci> struct X { /* ... */ };
int ai[10];
X<ai> xi; // array to pointer and qualification conversions

struct Y { /* ... */ };
template<const Y& b> struct Z { /* ... */ };
Y y;
Z<y> z; // no conversion, but note extra cv-qualification

template<int (&pa)[5]> struct W { /* ... */ };
int b[5];
W<b> w; // no conversion

void f(char);
void f(int);

template<void (*pf)(int)> struct A { /* ... */ };
A<&f> a; // selects f(int)

template<auto n> struct B { /* ... */ };
B<5> b1;   // OK: template parameter type is int
B<'a'> b2; // OK: template parameter type is char
// B<2.5> b3; // error: template parameter type cannot be double
