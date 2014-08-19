__declspec(selectany) int * pi1 = 0;   // OK, selectany & int both part of decl-specifier
int __declspec(selectany) * pi2 = 0;   // OK, selectany & int both part of decl-specifier

#if 0
int * __declspec(selectany) pi3 = 0;   // MSVC ERROR, selectany is not part of a declarator
#endif
