// Construction Rules for enum class Values

enum E : int {};
void f(E);
E e1 { 0 }; // OK
// E e2 = { 0 }; // error
E e3 = E{ 0 }; // OK

// const E& e4 { 0 }; // OK

// const E& e5 = { 0 }; // error

// f({ 0 }); // error

// f(E{ 0 }); // OK

// E g() { return { 0 }; // error
// }

struct X { E e { 0 }; // OK

  // E e6 = { 0 }; // error

X() : e { 0 } {} // OK
};

E* p = new E{ 0 }; // OK
