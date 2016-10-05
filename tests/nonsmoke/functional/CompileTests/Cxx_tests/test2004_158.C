
// Need to build test code to test different sorts of casts.
// I suspect that ROSE is unparsing all casts as a simple C cast (which is none to clever!)

// C style cast!
int x = 42;
// float y = (float) x;

#if 0
reinterpret_cast
const_cast
dynamic_cast
#endif

#define NULL 0L

class X {};
class Y : public X
   {
     public:
         Y();
         Y(double x);
   };

#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 4) )

Y* dynamic_yPtr;
const X* const_yPtr;
Y* static_yPtr;
// Y* xPtr1 = reinterpret_cast<X*>(yPtr);
X* xPtr2 = const_cast<X*>(const_yPtr);
X* xPtr3 = dynamic_cast<X*>(dynamic_yPtr);
X* xPtr4 = static_cast<X*>(static_yPtr);

static const int foo2 = static_cast<int>(1.0);

// void* malloc(int);

void foo()
   {
     void* malloc(int);
     int *ptr = static_cast<int*>(malloc(100));

     int x = reinterpret_cast<int>(ptr);

     float f;
     Y y(f);  // cast of function parameter
   }

#else
  #warning "Case not tested for version 3.4 and higher."
#endif

 
