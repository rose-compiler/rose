#if 1
#include<test2005_81.h>
#else
namespace std
   {
     template<typename T>
     struct X
        {
          T t;
        };

     template<typename T>
     void foobar(T t1)
        {
          T t2;
        };
   }
#endif

#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )
// GNU g++ does not allow the class to be specified with a qualified name but only 
// with a namespace of the same name using namespace std; // a using directive does 
// not help!
// ALSO, g++ version 3.4.x does not allow the class name to be qualified!
// while g++ version 3.3.x does allow it to be qualified (likely an error).
template<> struct std::X<int> { float x; };

// See if specialization of templated functions are just as much of a problem
template<> void std::foobar(float t) { float x; };
#else
  #warning "Case not tested for version 3.4 and higher."
#endif

#if 0
// GNU g++ allows this construction!
namespace std {
     template<> struct std::X<int> { float t; };
 }
#endif

// using namespace X;
void foo()
   {
     std::X<int> x;
     std::foobar(2.0);
   }

