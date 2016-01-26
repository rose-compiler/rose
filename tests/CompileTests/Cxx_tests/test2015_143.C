
// Consider the templated base class A:
template<typename T>
class A 
   {
     protected:
          T* ptr;
       // int i;
   };

// ... or qualifying the name with the this pointer
template<typename T>
class Bthis : public A<T> 
   {
     public:
       // T *getp() { return this->ptr; }
   };

// Again, a concrete subclass works as expected
class BconcreteSubclass : public Bthis<double> 
   {
  // using Bthis<double>::ptr;
     public:
          double *getp1() { return ptr; }
   };

