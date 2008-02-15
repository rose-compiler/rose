#ifndef X_D_H
#define X_D_H


//-----------------------------------------------------------------------------
// Define second order difference approximations:
// The include file cgux2a.h defines second order difference approximations
//
// Notes:
//  For efficiency we define UR2, US2 and UT2 to be the arrays that we have precomputed
//  the parametric derivatives in. (Otherwise the derivatives would get
//  recomputed for all derivatives). UR2, US2 and UT2 are used in cgux2a.h
//  to define UX2, UY2, UXX2, ...
//-----------------------------------------------------------------------------
#define U(I1,I2,I3,N)   u(I1,I2,I3,N)
#define UR2(I1,I2,I3,N) ur(I1,I2,I3,N)
#define US2(I1,I2,I3,N) us(I1,I2,I3,N)
#define UT2(I1,I2,I3,N) ut(I1,I2,I3,N)
#include "cgux2a.h"    // define 2nd order difference approximations

//-----------------------------------------------------------------------------
// Define fourth order difference approximations:
// The include file cgux4a.h defines fourth order difference approximations
//
// Notes : see above
//----------------------------------------------------------------------------
#define UR4(I1,I2,I3,N) ur(I1,I2,I3,N)
#define US4(I1,I2,I3,N) us(I1,I2,I3,N)
#define UT4(I1,I2,I3,N) ut(I1,I2,I3,N)
#include "cgux4a.h"    // define 4th order difference approximations

//------------------------------------------------------------------------------------
// This function defines derivatives for all derivative types
//   derivativeType : which derivative to compute
//   ux  : array to put the results in
//   4: order of accuracy 2 or 4
//   .nd   : number of dimensions 2 or 3
// Notes:
//  This macro calls one of the macros UX22, UX23, UX42, UX43 etc,
//  (defined in the include file cgux2a.h or cgux4a.h) to compute a 
//  derivative to first or second order accuracy
//------------------------------------------------------------------------------------


#endif
