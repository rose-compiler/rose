
// This test code is adapted from test2004_158.C (which only applied to older versions of GNU 3.4 and older).

// Need to build test code to test different sorts of casts.
// I suspect that ROSE is unparsing all casts as a simple C cast (which is none to clever!)

// C style cast!
int x = 42;
float y = (float) x;

// This is a functional notation cast.
float z = float(x);

// Also testing:
//    reinterpret_cast
//    const_cast
//    dynamic_cast
//    const_cast

#define NULL 0L

class X {};
class Y : public X
   {
     public:
         Y();
         Y(double x);
   };

Y* yPtr;
Y* dynamic_yPtr;
const X* const_yPtr;
Y* static_yPtr;

// Make the converted types more different.
// void* xPtr1 = reinterpret_cast<Y*>(yPtr);
void* xPtr1 = reinterpret_cast<Y*>(42);

X* xPtr2    = const_cast<X*>(const_yPtr);
X* xPtr3    = dynamic_cast<X*>(dynamic_yPtr);
X* xPtr4    = static_cast<X*>(static_yPtr);

static const int foo2 = static_cast<int>(1.0);

// void* malloc(int);

void foo()
   {
     void* malloc(int);
     int *ptr = static_cast<int*>(malloc(100));

  // Loss of precission causes an error in later version of GNU g++.
  // int x = reinterpret_cast<int>(ptr);
  // unsigned int x = reinterpret_cast<unsigned int>(ptr);
  // unsigned long x = reinterpret_cast<unsigned long>(ptr);
     long x = reinterpret_cast<long>(ptr);

     float f;
     Y y(f);  // cast of function parameter
   }

 
