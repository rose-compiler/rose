/*
Bug report from Qing:

I attached the reduced test code that shows the bug. The type of
"lsdremi (Generic_Index)" is SgTypeDefault  (in
the last statement:

lsdremi (Generic_Index) = gamma * prsremi (Generic_Index) * rhoremi (Generic_Index); )

It doesn't happen if "SimpleA++.h" is included instead of "A++.h", so it
must have something to do with including
A++.h

Thanks.
Qing
*/

// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )
#if ( defined(__clang__) == 0 && __GNUC__ == 3 )

#include <A++.h>
//#include "simpleA++.h"

#include <time.h>


// A++/P++ translation of FORTRAN function for Dinshaw's code!

void One_Dimensional_Goduov_Solver ( int nriemann,
                double prsfac ,
                double rhofloor ,
                double prsfloor ,
                double gamma ,
                int pprieman ,
                int rieiter ,
     doubleArray & rhoremi, doubleArray & rhorepl,
     doubleArray & prsremi, doubleArray & prsrepl,
     doubleArray & vlxremi, doubleArray & vlxrepl,
     doubleArray & vlyremi, doubleArray & vlyrepl,
     doubleArray & vlzremi, doubleArray & vlzrepl,
     doubleArray & rhoflux, doubleArray & engflux,
     doubleArray & vlxflux, doubleArray & vlyflux, 
     doubleArray & vlzflux,
     doubleArray & rho0, doubleArray & prs0, doubleArray & vlx0,
     doubleArray & vly0, doubleArray & vlz0, doubleArray & sdsp0 )
   {
     doubleArray lsdremi  (nriemann+2);
     doubleArray lsdrepl  (nriemann+2);
     doubleArray spdbarmi (nriemann+2);
     doubleArray spdbarpl (nriemann+2);
     doubleArray prsbar   (nriemann+2);
     doubleArray oprsbar  (nriemann+2);
     doubleArray rhobarmi (nriemann+2);
     doubleArray rhobarpl (nriemann+2);
     doubleArray sdbarmi  (nriemann+2);
     doubleArray sdbarpl  (nriemann+2);
     doubleArray vlxbar   (nriemann+2);

     doubleArray wavspd1  (nriemann+2);
     doubleArray wavspd2  (nriemann+2);
     doubleArray wavspd3  (nriemann+2);
     doubleArray wavspd4  (nriemann+2);
     doubleArray wavspd5  (nriemann+2);

     lsdremi  = 0;
     lsdrepl  = 0;
     spdbarmi = 0;
     spdbarpl = 0;
     prsbar   = 0;
     oprsbar  = 0;
     rhobarmi = 0;
     rhobarpl = 0;
     sdbarmi  = 0;
     sdbarpl  = 0;
     vlxbar   = 0;
     wavspd1  = 0;
     wavspd2  = 0;
     wavspd3  = 0;
     wavspd4  = 0;
     wavspd5  = 0;

     int i       = 0;
     int ii      = 0;
     int numconv = 0;
     int ncnew   = 0;

// ********************************************************************

  // Note that "prsbar()" is derived from a purely mechanical condition
  // and doesn't depend on equation of state. The sound speeds of course
  // do depend on an eos.

  // Fix bottom on density and pressure variables.
     Index Generic_Index ( 1 , nriemann, 1 );  // The name is special request from Dinshaw!

     lsdremi (Generic_Index) = gamma * prsremi (Generic_Index) * rhoremi (Generic_Index);

}

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

