template<class T>
struct A
   {
     struct X {};
   };

template<class T>
struct B : public A<T>
   {
     using typename A<T>::X;
   };

