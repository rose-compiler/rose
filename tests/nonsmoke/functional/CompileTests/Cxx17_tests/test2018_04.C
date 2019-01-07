
template<const int* pci> struct X { /* ... */ };

int ai[10];

X<ai> xi; // array to pointer and qualification conversions
