// This is a modified version of test2001_10.C used for debugging details of the EDG 4.3 support in ROSE.

/*
   Constructor call not unparsed correctly

Original code:
   return doubleArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
Unparsed code:
   return (New_Data_Pointer,Vectorized_Domain_Pointer);
 */

#if 1
class Domain
   {
     public:
          Domain (int i);
   };
#endif

class A
   {
     public:
#if 0
          A (int i);
          A (int i, int j);
#endif
#if 1
          A (double* dataPtr, Domain X);
#endif

// EDG 4.3 version of ROSE does not handle the const well...
//        A operator()();
          A operator()() const;
   };

#if 0
Domain::Domain( int i)
   {
   }

A::A( int i)
   {
   }

A::A( int i, int j)
   {
   }

A::A (double* dataPtr, Domain X)
   {
   }
#endif

#if 1
// EDG 4.3 version of ROSE does not handle the const well...
// A A::operator()()
A A::operator()() const
   {
     int i = 999;
     double* xPtr = 0x0000;
     return A(xPtr,2);
   }
#endif

#if 0
int
main ()
   {
     return 0;
   }
#endif

