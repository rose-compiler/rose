template<class X> void f(X a);  // first template f
template<class X> void f(X* a); // second template f
template<> void f<>(int *a) {} // explicit specialization
 // template argument deduction comes up with two candidates:
 // foo<int*>(int*) and f<int>(int*)
 // partial ordering selects f<int>(int*) as more specialized


