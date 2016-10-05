

#include "indirectAddressingSupport.h"


int main()
   {
     int size = 10;

     double* A = IndirectAddressingSupport::generateDataArray(size);
     double* B = IndirectAddressingSupport::generateDataArray(size);
     int* I    = IndirectAddressingSupport::generateDataArray(size);

     for (int i=0; i < size; i++)
        A[I[i]] = B[I[i]] - dt * (B[I[i]] + B[I[i] - 1]) / dx;

  // Optimized code is:

  // Transformation which does not require analysis
  // Conditions:
  //    A and B are not aliased
     int* Iprime = IndirectAddressingSupport::copyIndirectionArray(I,size);
     IndirectAddressingSupport::sortIndirectionArray(Iprime,size);
     for (int i=0; i < size; i++)
        A[I[Iprime[i]]] = B[I[Iprime[i]]] - dt * (B[I[Iprime[i]]] + B[I[Iprime[i]] - 1]) / dx;

  // Transformation which requires more analysis
  // Conditions:
  //    Indirection vector, I, does not chenge between different loops over A and B
  //    A and B are not aliased

     for (int i=0; i < size; i++)
        A[Iprime[i]] = B[Iprime[i]] - dt * (B[Iprime[i]] + B[Iprime[i] - 1]) / dx;

     return 0;
   }

