/*
   Constructor call not unparsed correctly

Original code:
   return doubleArray ( New_Data_Pointer , Vectorized_Domain_Pointer );
Unparsed code:
   return (New_Data_Pointer,Vectorized_Domain_Pointer);
 */

class Domain
   {
     public:
          Domain (int i);
   };

class A
   {
     public:
          A (int i);
          A (int i, int j);
          A (double* dataPtr, Domain X);
          A operator()() const;
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

A
A::operator()() const
   {
     double* xPtr = 0;
     return A(xPtr,2);
   }

int
main ()
   {
     return 0;
   }

