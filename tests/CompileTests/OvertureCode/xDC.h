#ifndef X_D_C_H
#define X_D_C_H

#include "MappedGridOperators.h"

//-----------------------------------------------------------------------------
//  Define the routines that return the coefficients of the finite difference
//  operators.
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


// MERGE0 : use for A++ operations when the first index is a scalar
//      a(i0,I1,I2,I3)=
// you must define the following in your code
//  int dum;
//  Range aR0,aR1,aR2,aR3;
#define MERGE0(a,i0,I1,I2,I3) \
  for(  \
      aR0=Range(a.getBase(0),a.getBound(0)),   \
      aR1=Range(a.getBase(1),a.getBound(1)),   \
      aR2=Range(a.getBase(2),a.getBound(2)),   \
      aR3=Range(a.getBase(3),a.getBound(3)),   \
      a.reshape(Range(0,aR0.length()*aR1.length()-1),aR2,aR3), \
      dum=0; dum<1; dum++,  \
      a.reshape(aR0,aR1,aR2,aR3) ) \
    a(Index(i0-aR0.getBase()+aR0.length()*(I1.getBase()-aR1.getBase()),   \
      I1.length(),aR0.length()),I2,I3)


#define M123(m1,m2,m3) (m1+halfWidth1+width*(m2+halfWidth2+width*(m3+halfWidth3)))
#define M123N(m1,m2,m3,n) (M123(m1,m2,m3)+stencilSize*(n))

// Use this for indexing into coefficient matrices representing systems of equations
#define CE(c,e) (stencilSize*((c)+numberOfComponentsForCoefficients*(e)))
#define M123CE(m1,m2,m3,c,e) (M123(m1,m2,m3)+CE(c,e))

// M123 with a fixed offset
#define MCE(m1,m2,m3) (M123(m1,m2,m3)+CE(c0,e0))
// define COEFF(I1,I2,I3,m1,m2,m3) coeff(I1,I2,I3,MN(m1,m2,m3))
// define EQUATIONNUMBER(I1,I2,I3,m1,m2,m3) equationNumber0(I1,I2,I3,MN(m1,m2,m3))

// Use this for A++ index operations:
#undef  UX_MERGED
#define UX_MERGED(m1,m2,m3,c,e,I1,I2,I3) MERGE0(derivative,M123CE(m1,m2,m3,c,e),I1,I2,I3)
#undef  EQUATIONNUMBER
#define EQUATIONNUMBER(m1,m2,m3,n,I1,I2,I3) MERGE0(equationNumber0,M123N(m1,m2,m3,n),I1,I2,I3)
// Scalar indexing:
#define COEFFS(m1,m2,m3,n,I1,I2,I3) coeff0(M123N(m1,m2,m3,n),I1,I2,I3)
#define EQUATIONNUMBERS(m1,m2,m3,n,I1,I2,I3) equationNumber0(M123N(m1,m2,m3,n),I1,I2,I3)


#define ForStencil(m1,m2,m3)   \
    for( m3=-halfWidth3; m3<=halfWidth3; m3++) \
    for( m2=-halfWidth2; m2<=halfWidth2; m2++) \
    for( m1=-halfWidth1; m1<=halfWidth1; m1++) 

#define ForStencilN(n,m1,m2,m3)   \
    for( n=0; n<numberOfComponents; n++) \
    for( m3=-halfWidth3; m3<=halfWidth3; m3++) \
    for( m2=-halfWidth2; m2<=halfWidth2; m2++) \
    for( m1=-halfWidth1; m1<=halfWidth1; m1++) 



// Define second order difference approximations:
#undef U
#undef UR2
#undef US2
#undef UT2
#undef URR2
#undef URS2
#undef URT2
#undef USS2
#undef UST2
#undef UTT2
#define U(I1,I2,I3,N)     (delta(m1) *delta(m2) *delta(m3))
#define UR2(I1,I2,I3,N)   (Dr(m1)    *delta(m2) *delta(m3))
#define US2(I1,I2,I3,N)   (delta(m1) *Ds(m2)    *delta(m3))
#define UT2(I1,I2,I3,N)   (delta(m1) *delta(m2) *Dt(m3))
#define URR2(I1,I2,I3,N)  (Drr(m1)   *delta(m2) *delta(m3))
#define URS2(I1,I2,I3,N)  (Dr(m1)    *Ds(m2)    *delta(m3))
#define URT2(I1,I2,I3,N)  (Dr(m1)    *delta(m2) *Dt(m3))
#define USS2(I1,I2,I3,N)  (delta(m1) *Dss(m2)   *delta(m3))
#define UST2(I1,I2,I3,N)  (delta(m1) *Ds(m2)    *Dt(m3))
#define UTT2(I1,I2,I3,N)  (delta(m1) *delta(m2) *Dtt(m3))
// Here are the defines for 4th order operators used in cgux4a.h
#undef UR4
#undef US4
#undef UT4
#undef URR4
#undef URS4
#undef URT4
#undef USS4
#undef UST4
#undef UTT4
#define UR4(I1,I2,I3,N)   (Dr4(m1)   *delta(m2) *delta(m3))
#define US4(I1,I2,I3,N)   (delta(m1) *Ds4(m2)   *delta(m3))
#define UT4(I1,I2,I3,N)   (delta(m1) *delta(m2) *Dt4(m3))
#define URR4(I1,I2,I3,N)  (Drr4(m1)  *delta(m2) *delta(m3))
#define URS4(I1,I2,I3,N)  (Dr4(m1)   *Ds4(m2)   *delta(m3))
#define URT4(I1,I2,I3,N)  (Dr4(m1)   *delta(m2) *Dt4(m3))
#define USS4(I1,I2,I3,N)  (delta(m1) *Dss4(m2)  *delta(m3))
#define UST4(I1,I2,I3,N)  (delta(m1) *Ds4(m2)   *Dt4(m3))
#define UTT4(I1,I2,I3,N)  (delta(m1) *delta(m2) *Dtt4(m3))

#endif
