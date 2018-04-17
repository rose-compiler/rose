// This is a bug demonstrated by BoxLib.
// Namely, the ROSE generated code contains errors.

// The code below demonstrates the errors, it is required to 
// seperate the declarations into 2 different copies of the 
// "std" namespace.

int x;

#if 1
/* Review this issue later (problem with MSVC support for reference to un-named enum types in template declaration). */
namespace std
   {
   }


namespace std
   {
     template < typename _Tp >
     struct __is_void
        {
          enum { __value = 0 };
        };

     template<> struct __is_void < void >  
        {
          enum {__value=1};
        };
   }
#endif

