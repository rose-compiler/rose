
template <int   i> class Xi {};

#if 0
template <int * i> class Yi {};
template <int & i> class Zi {};
#endif

#if 0
// template <double   d> class X; // Error
template <double * d> class Y {};
template <double & d> class Z {};
#endif

int main()
   {
     int i;
     Xi<42> x;

#if 0
     int *iPtr = &i;
     Yi<iPtr> y;

     int & iRef = i;
     Zi<iRef> z;
#endif

#if 0
     X<4.0> x;
     double* dptr = 0L;
     Y<dptr> y;

     const double dValue = 0.0;
     Z<0.0> z;
#endif

     return 0;
   }
