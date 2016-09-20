
// Handle case of variable declaration from different templated classes
// variable declarations can require qualified names!  This is not as
// complex a case as where an extern (secondary) declaration is placed 
// within a class.


struct A
   {
     static const int value;
     unsigned int:16;
   };

namespace X
   {
     extern int externValue;
   }

template<typename T>
struct B
   {
     static const int value;
   };

// Static data member initialization of class works!
const int A::value = 0;

// Static data member initialization of instantiated template class fails!
// const int B<int>::value = 0;
#ifdef __GNUC_PREREQ
#if (__GNUC_PREREQ(3,4) == 0)
  // Works for g++ 3.3.2 compiler but not g++ 3.4.3
     const int B<int>::value = 0;
#endif
#endif

// Extern data member initialization of namespace fails!
int X::externValue = 0;



