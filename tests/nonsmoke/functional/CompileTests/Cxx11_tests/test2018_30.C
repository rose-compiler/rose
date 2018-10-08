// Unparser error in rose 0.9.9.192 identityTranslator. Symptom is error like :

// rose_foo.cc(20): error: the #if for this directive is missing
// #endif
// ^

// This can be reproduced with the code:

struct struct1 {};
struct struct2 {};

template < typename typename1 = struct1,
typename typename2 >
inline void template_func1( typename2&& parm1 ) {}

void func2() {
template_func1<struct2>([=](int parm2)
{ 
#ifndef UNDEFINED_VARIABLE
}

);
#endif
}
