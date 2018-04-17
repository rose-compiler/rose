// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )

enum values { zero, nonzero };

class A
   {
     public:
          enum values { zero, nonzero };
   };

#if 1
template <typename T>
class X 
   {
   };
#endif

void foo()
   {
     X<values> x1;
     X<A::values> x2;
     X<A> x3;

     X<A::values const & > x4;
   }

// #else
//   #warning "Not tested on gnu 4.0 or greater versions"
// #endif
