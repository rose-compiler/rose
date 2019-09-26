// Removing Deprecated Exception Specifications from C++17

void (*p)() throw(int);
void (**pp)() noexcept = &p; // error: cannot convert to pointer to noexcept function

struct S { typedef void (*p)(); operator p(); };
void (*q)() noexcept = S(); // error: cannot convert to pointer to noexcept function

