#include "shared.h"
template <class Type> void myclass<Type>::setx(Type y) { x = y; }
template <class Type> Type myclass<Type>::getx() { return x; }

// must trigger instantiation of the class, otherwise template functions are ignored in a library
// using concrete types to instantiate some class
template class myclass<int>;

