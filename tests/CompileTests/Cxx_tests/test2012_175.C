

template <typename T> 
struct X 
   {
     struct Y;
     Y y1;
   };

// Template arguments in the qualified name for a nested class in a template.
template <typename T> struct X<T>::Y {};

// This is an error:
// template <typename T> struct X::Y {};

X<int> a;

void
foo()
   {
     X<int> a;
   }

