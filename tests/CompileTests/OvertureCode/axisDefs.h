/*  -*-Mode: c++; -*-  */
// Bill's useful definitions and some more
#ifndef AXISDEFS_H
#define AXISDEFS_H "axisdefs.h"
#  ifndef WDHDEFS_H
#  define WDHDEFS_H "wdhdefs.h"
//#extern int debug;     // variable used for debugging
//970520: change for SGI
extern int debug;     // variable used for debugging

const int axis1 = 0;  // names for the 3 parameter axes
const int axis2 = 1;
const int axis3 = 2;

const int xAxis = 0;  // names for the 3 cartesian coordinate directions
const int yAxis = 1;
const int zAxis = 2;

const int Start = 0;  // names for left/right top/bottom back/front
const int End   = 1;

const REAL Pi = 4.*atan(1.);
const REAL twoPi = 8.*atan(1.);
#  endif // WDHDEFS_H

const int rAxis = 0;
const int sAxis = 1;
const int tAxis = 2;

const int xComponent = 0;
const int yComponent = 1;
const int zComponent = 2;

const int rComponent = 0;
const int sComponent = 1;
const int tComponent = 2;

const int uComponent = 0;
const int vComponent = 1;
const int wComponent = 2;

const int noAxis = -1;
const int noComponent = -1;
const int allAxes = -2;
const int allComponents = -2;

const int oneDimension = 1;
const int twoDimensions = 2;
const int threeDimensions = 3;

#endif // AXISDEFS_H
