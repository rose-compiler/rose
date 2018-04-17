// template argument default value from another template class

// originally found in package buffy

// Assertion failed: newHead->next == NULL, file ../ast/fakelist.h line 81

// ERR-MATCH: Assertion failed: newHead->next

template <typename T> struct B {
    static const int value = true;
};

struct T1 {
};

template <class U, int v = B<T1>::value>
struct S {
};

struct U{};
S<U> x;
