// This and example showing the static const integer data member in-class initialization!
// It does not work for non-iteger types (like float or double), but it does work for
// char, long, etc.

// Under some non-standard conditions doubles and floats can be initialized in-class
// though I think it is handled sufficiently differently in different compilers o make 
// it painful.
// More info at:
//      http://www.cqf.info/forum/viewtopic.php?t=876&view=previous
//      C++ static double in class

// #include<stdlib.h>

class X
   {
     public:
       // Notice that we can initialize static constants within the class!
          static const int    maxIntValue    = 3;
          static const long   maxLongValue   = 3;
          static const char   maxCharValue   = 'Z';

       // Not allowed for float, double, or pointers (interesting!)
       // static const float  maxFloatValue  = 3.0;
       // static const double maxDoubleValue = 3.0;
       // static const size_t maxSizeTValue = 3;
       // static const int   *maxIntPointerValue = 0L;

       // types of data members that can't be initalized in-class
          static const double pi;
          const int tuncatedPi;
          const double e;

       // this is the only way to initalize a non-static const variable (integer or double)
          X(): tuncatedPi(3), e(2.71) {}
   };

// This is the only way to initialize a static const data member which is non-integer based
const double X::pi = 3.14;


#if 0
#ifndef USE_ROSE
#define STORAGE static
#else
#define STORAGE
#endif
#endif

// #if SWIG
#if 0
#ifndef __GNUC__
#define STORAGE static
#else
#define STORAGE
#endif

namespace Y 
   {
#else
#define STORAGE
class Y
   {
     public:
#endif
       // This is allowed by GNU but not by EDG
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // This is allowed by EDG, but not by g++ (g++ needs constant to be static)
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // Code that will compile with EDG
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places
       // Code that we should generate so that we can compile with g++
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

          STORAGE const double pi = 3.141592653589793238462643383279; // Pi to 30 places
   };


void foo()
   {
     X x, *xptr;

     const double gamma        = 7.89;
     const int    integerConst = 42;

     double var1  = X::pi;
     double var2  = x.e;
     double var3  = gamma;
     int    var4  = integerConst;

  // Access via static qualifier works fine but access via data member filed generates error: "x->3;"
  // The fix (to EDG/Sage translation) was to make these generate the same code (as it should be)
     int    var5  = X::maxIntValue;
     int    var6  = x.maxIntValue;

     int    var7  = x.maxLongValue;
     char   var8  = x.maxCharValue;

     int    var9  = xptr->maxIntValue;
     double var10 = (double) (double) xptr->pi;

  // ROSE can properly handle integer constants but we need to handle
  // floating point constants as well (which is non-standard C++).
  // Since we are close this is likely worth fixing.
     Y y,*yptr;
     double var11 = Y::pi;     // This works
     double var12 = y.pi;      // This does not work presently
     double var13 = yptr->pi;  // This does not work presently
   }
