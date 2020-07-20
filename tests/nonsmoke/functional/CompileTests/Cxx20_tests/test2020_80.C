template<class X> void f(X a);
template<class X> void f(X* a);
void (*p)(int*) = &f;


