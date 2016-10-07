// This file was submitted by Kyle Chand

// Consider the templated base class A:
template<typename T>
class A
   {
     protected:
          T* ptr;
          int i;
   };

#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
// ... or qualifying the name with the this pointer
template<typename T>
class Bthis : public A<T>
   {
     public:
          T *getp() { return this->ptr; }
   };

// ... and just in case instantiation causes any other troubles ...
void foo()
   {
  // A<int> a;
     Bthis<int> bt;
   }
#else
  #warning "Case not tested for version 3.4 and higher."
#endif

