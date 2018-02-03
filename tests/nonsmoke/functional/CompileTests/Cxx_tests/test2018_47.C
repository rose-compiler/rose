
template <class T>
struct A
   {
     static T x;
   };

// Declare space for static variable.
template <class T> T A<T>::x = 0;

// Template instantiation directive.
// template int A<int>::x;
