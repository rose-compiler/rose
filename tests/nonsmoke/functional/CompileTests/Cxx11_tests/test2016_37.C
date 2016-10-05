
#include <cstddef>

// This is the most elegant solution, if nullptr_t is defined in std, but this is not what is used.
// using namespace std;

// This is an alternative solution for older compilers, but this will evaluate 
// differently for ROSE using the Intel compiler as the backend because the 
// Intel compiler support in ROSE defines the __GNUC__ and other macros.
// So what we want to fix in ROSE is the degree to which we are emulating the 
// GNU compiler when the Intel compiler is selected as the backend compiler.
#ifdef __GNUC__
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#else
#define GCC_VERSION 0
#endif

#if GCC_VERSION < 40700

#warning "Using user defined nullptr_t type."

class nullptr_t
   {
     public:
          nullptr_t() { }
          template<typename T> operator T*() const { return 0; }
          template<typename S, typename T> operator T S::*() { return 0; }
     private:
          void operator&();
   };

#else

// I think we need this when the macro predicate is false.

#warning "Using nullptr_t type from std."

// using namespace std;

#endif


template <class T>
class X
   {
     public:
          X<T>& operator= (T* from)
             {
               return (*this);
             }

          X<T>& operator= (nullptr_t from)
             {
               return *this;
             }
   };
