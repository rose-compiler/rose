// Test to verify that specializations of the template class and template
// member function show up as unparsed code if we force 
// bool Unparser::isTransformed(SgStatement* stmt)
// to return true.  They should not if it is forced to be false.

template <typename T>
class example
   {
     public:
          T t;
       // This should not be unparsed with "template<>" as a prefix
          int foo() { return 0; }
   };

// template <class T> inline void destroy(T* __pointer) { __pointer->~T(); }
template <class T> void destroy(T* __pointer) { __pointer->~T(); }

enum values { x,y };


int main()
   {
     example<int> e;
     int x = e.foo();

  // destroy<int>(&x);
#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     destroy<int>(&x);
#endif
#endif

     values* selectionPtr = 0;
     destroy<values>(selectionPtr);
   }

