/* wdh: add this next to define Boxlib define's as a function of Overture define's */
#include "OvertureBL.h"

#ifndef _constants_H_
#define _constants_H_

/*
** $Id: CONSTANTS.H,v 1.1 2004/07/07 10:27:01 dquinlan Exp $
*/

#ifdef BL_BACKWARD_COMPAT
#if defined(BL_LANG_CC) || !defined(BL_LANG_FORT)

#define BIG_REAL 1.0e+30
#define BIGREAL  1.0e+30
#define ZERO     0.0
#define ONE      1.0
#define TWO      2.0
#define THREE    3.0
#define FOUR     4.0
#define FIVE     5.0
#define SIX      6.0
#define SEVEN    7.0
#define EIGHT    8.0
#define NINE     9.0
#define TEN      10.0
#define TWELVE   12.0
#define FIFTEEN  15.0
#define SIXTEEN  16.0
#define TWENTY   20.0
#define TENTH    0.1
#define EIGHTH   0.125
#define SIXTH    0.16666666666667
#define FORTH    0.25
#define FOURTH   0.25
#define THIRD    0.33333333333333
#define HALF     0.5
#define TWO3RD   0.66666666666667
#define PI       3.14159265358979

#endif /*defined(BL_LANG_CC) || !defined(BL_LANG_FORT)*/
#endif /*BL_BACKWARD_COMPAT*/

#ifdef BL_LANG_FORT

#ifdef BL_USE_FLOAT
#define bigreal 1.0e+30
#define zero    0.0
#define one     1.0
#define two     2.0
#define three   3.0
#define four    4.0
#define five    5.0
#define six     6.0
#define seven   7.0
#define eight   8.0
#define nine    9.0
#define ten     10.0
#define twelve  12.0
#define fifteen 15.0
#define sixteen 16.0
#define twenty  20.0
#define tenth   0.1
#define eighth  0.125
#define sixth   0.16666666666667
#define forth   0.25
#define fourth  0.25
#define third   0.33333333333333
#define half    0.5
#define two3rd  0.66666666666667
#define Pi      3.14159265358979

#endif /*BL_USE_FLOAT*/

#ifdef BL_USE_DOUBLE

#define bigreal 1.0d+30
#define zero    0.0d0
#define one     1.0d0
#define two     2.0d0
#define three   3.0d0
#define four    4.0d0
#define five    5.0d0
#define six     6.0d0
#define seven   7.0d0
#define eight   8.0d0
#define nine    9.0d0
#define ten     10.0d0
#define twelve  12.0d0
#define fifteen 15.0d0
#define sixteen 16.0d0
#define twenty  20.0d0
#define tenth   0.1d0
#define eighth  0.125d0
#define sixth   0.16666666666667d0
#define forth   0.25d0
#define fourth  0.25d0
#define third   0.33333333333333d0
#define half    0.5d0
#define two3rd  0.66666666666667d0
#define Pi      3.14159265358979d0

#endif /*BL_USE_DOUBLE*/

#endif /*BL_LANG_FORT*/

#endif /*_constants_H_*/
