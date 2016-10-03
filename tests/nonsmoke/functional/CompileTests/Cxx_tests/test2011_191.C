
template<typename T>
class A
   {
     public:
          T foo();
          T abc;
   };


template<typename T>
class B // : public iterator_traits<_Iterator>
   {
     using ::A<T>::abc;
  // using ::A<T>;
   };
