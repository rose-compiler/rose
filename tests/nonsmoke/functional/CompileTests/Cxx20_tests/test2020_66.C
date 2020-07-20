template<typename T>
concept Decrementable = requires(T t) { --t; };
template<typename T>
concept RevIterator = Decrementable<T> && requires(T t) { *t; };
 
// RevIterator subsumes Decrementable, but not the other way around
 
template<Decrementable T>
void f(T); // #1
 
template<RevIterator T>
void f(T); // #2, more constrained than #1
 
f(0);       // int only satisfies Decrementable, selects #1
f((int*)0); // int* satisfies both constraints, selects #2 as more constrained
 
template<class T>
void g(T); // #3 (unconstrained)
 
template<Decrementable T>
void g(T); // #4
 
g(true);  // bool does not satisfy Decrementable, selects #3
g(0);     // int satisfies Decrementable, selects #4 because it is more constrained
 
template<typename T>
concept RevIterator2 = requires(T t) { --t; *t; };
 
template<Decrementable T>
void h(T); // #5
 
template<RevIterator2 T>
void h(T); // #6
 
h((int*)0); //ambiguous


