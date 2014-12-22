typedef struct {} T;
void f(T p, decltype(p));                // L = 1
void g(T p, decltype(p) (*)());          // L = 1
void h(T p, auto (*)()->decltype(p));    // L = 1
void i(T p, auto (*)(T q)->decltype(q)); // L = 0
void j(T p, auto (*)(decltype(p))->T);   // L = 2
void k(T p, int (*(*)(T p))[sizeof(p)]); // L = 1
