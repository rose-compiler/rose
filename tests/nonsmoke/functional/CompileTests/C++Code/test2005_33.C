
class Y
   {
     public:
       // This is allowed by GNU but not by EDG
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // This is allowed by EDG, but not by g++ (g++ needs constant to be static)
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // Code that will compile with EDG
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places
       // Code that we should generate so that we can compile with g++
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

          const double x;
          static const double y;
          const double pi = 3.141592653589793238462643383279; // Pi to 30 places

          Y() : x(3.14) {}

       // End of class definition!
   };

const double Y::y = 2.71;

class Z
   {
  // Nothing to put in this class ...
   };

#if 1
void foo()
   {
  // ROSE can properly handle integer constants but we need to handle
  // floating point constants as well (which is non-standard C++).
  // Since we are close this is likely worth fixing.
     Y y,*yptr;
     double var1 = Y::pi;     // This works
     double var2 = y.pi;      // This does not work presently
     double var3 = yptr->pi;  // This does not work presently
   }
#endif
