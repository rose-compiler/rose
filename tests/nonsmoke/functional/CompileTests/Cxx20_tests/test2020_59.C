// DQ (7/21/2020): Concept support is not available in EDG 6.0.

template<class T>
constexpr bool is_meowable = true;
 
template<class T>
constexpr bool is_purrable() { return true; }
 
template<class T>
void f(T) requires is_meowable<T>; // OK
 
template<class T>
void g(T) requires is_purrable<T>(); // error, is_purrable<T>() is not a primary expression
 
template<class T>
void h(T) requires (is_purrable<T>()); // OK


