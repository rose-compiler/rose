// Restricted to versions of GNU 3.4 and earlier
// The ../A++Code/partitioning.h file is a problem for GNU g++ 4.2.2
#if ( (__GNUC__ == 3) && (__GNUC_MINOR__ < 5) )

#include "A++.h"
// #include <glob.h>

#if 0

#if 1
extern "C"
   {
/* machine.h is found in MDI/machine.h through a link in A++/inc lude and P++/inc lude */
#include "machine.h"
   }
#endif

#define WORK_AROUND 0

class Domain
   {
     public:
          Domain (int i);
   };

class A
   {
     public:
          A ();
          A (int i);
          A (int i, int j);
          A (double* dataPtr, Domain X);
#if WORK_AROUND
          A operator()() const;
#else
          doubleArray operator()() const;
#endif
   };

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

#if WORK_AROUND
A
A::operator()() const
   {
     double* xPtr = 0;
     return A(xPtr,2);
   }
#else
doubleArray A::operator() () const
   {
     double* New_Data_Pointer = NULL;
     Array_Domain_Type* Vectorized_Domain_Pointer = NULL;

  // Test 5
     return doubleArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
   }
#endif

int
main ()
   {
     return 0;
   }

#endif

// Restricted to versions of GNU 3.4 and earlier
#endif
