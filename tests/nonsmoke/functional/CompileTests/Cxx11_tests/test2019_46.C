template<class T>
struct A
   {
     struct X {};
   };

template<class T>
struct B 
   {
     using typename A<T>::X;
   };

