// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<typename T>
void f(T&&) requires Eq<T>; // can appear as the last element of a function declarator
 
template<typename T> requires Addable<T> // or right after a template parameter list
T add(T a, T b) { return a + b; }


