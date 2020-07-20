template<Incrementable T>
void f(T) requires Decrementable<T>;
 
template<Incrementable T>
void f(T) requires Decrementable<T>; // OK, redeclaration
 
template<typename T>
    requires Incrementable<T> && Decrementable<T>
void f(T); // Ill-formed, no diagnostic required
 
// the following two declarations have different constraints:
// the first declaration has Incrementable<T> && Decrementable<T>
// the second declaration has Decrementable<T> && Incrementable<T>
// Even though they are logically equivalent.
 
template<Incrementable T> 
void g(T) requires Decrementable<T>;
 
template<Decrementable T> 
void g(T) requires Incrementable<T>; // ill-formed, no diagnostic required

