template <class T> decltype(g(T())) h(); // decltype(g(T())) is a dependent type
int g(int);
template <class T> decltype(g(T())) h() { // redeclaration of h() uses earlier lookup
    return g(T());                     // ...although the lookup here does find g(int)
}
int i = h<int>();   // template argument substitution fails; g(int)
                    // was not in scope at the first declaration of h()

