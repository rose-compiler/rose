// Bill's useful definitions
#ifndef WDHDEFS_H
#define WDHDEFS_H "wdhdefs.h"

#include "OvertureDefine.h"
#include <A++.h>
#include "aString.H"

extern int debug;     // variable used for debugging

const int axis1 = 0;  // names for the 3 parameter axes
const int axis2 = 1;
const int axis3 = 2;

const int xAxis = 0;  // names for the 3 cartesian coordinate directions
const int yAxis = 1;
const int zAxis = 2;

const int Start = 0;  // names for left/right top/bottom back/front
const int End   = 1;


#if !defined(OV_BOOL_DEFINED) && !defined(USE_PPP)
  typedef int bool;    // this will be the new standard for Boolean
#endif

#ifdef OV_EXCEPTIONS_NOT_SUPPORTED
  #define throw exit(1); cout << 
#endif

#define sprintf2(a,b) (sprintf(a,b),a)
#define sprintf3(a,b,c) (sprintf(a,b,c),a)
#define sprintf4(a,b,c,d) (sprintf(a,b,c,d),a)
#define sprintf5(a,b,c,d,e) (sprintf(a,b,c,d,e),a)
#define sprintf6(a,b,c,d,e,f) (sprintf(a,b,c,d,e,f),a)
#define sprintf7(a,b,c,d,e,f,g) (sprintf(a,b,c,d,e,f,g),a)
#define sprintf8(a,b,c,d,e,f,g,h) (sprintf(a,b,c,d,e,f,g,h),a)
#define sprintf9(a,b,c,d,e,f,g,h,i) (sprintf(a,b,c,d,e,f,g,h,i),a)

// sprintf like routine that returns the formated string s
extern char* sPrintF(char *s, const char *format, ...);
extern aString& sPrintF(aString & s, const char *format, ...);

extern int 
sScanF(const aString & s, const char *format, 
       void *p0,
       void *p1=NULL, 
       void *p2=NULL, 
       void *p3=NULL,
       void *p4=NULL,
       void *p5=NULL,
       void *p6=NULL,
       void *p7=NULL,
       void *p8=NULL,
       void *p9=NULL,
       void *p10=NULL,
       void *p11=NULL,
       void *p12=NULL,
       void *p13=NULL,
       void *p14=NULL,
       void *p15=NULL,
       void *p16=NULL,
       void *p17=NULL,
       void *p18=NULL,
       void *p19=NULL,
       void *p20=NULL,
       void *p21=NULL,
       void *p22=NULL,
       void *p23=NULL,
       void *p24=NULL,
       void *p25=NULL,
       void *p26=NULL,
       void *p27=NULL,
       void *p28=NULL,
       void *p29=NULL );

extern int 
fScanF(FILE *file, const char *format, 
       void *p0,
       void *p1=NULL, 
       void *p2=NULL, 
       void *p3=NULL,
       void *p4=NULL,
       void *p5=NULL,
       void *p6=NULL,
       void *p7=NULL,
       void *p8=NULL,
       void *p9=NULL,
       void *p10=NULL,
       void *p11=NULL,
       void *p12=NULL,
       void *p13=NULL,
       void *p14=NULL,
       void *p15=NULL,
       void *p16=NULL,
       void *p17=NULL,
       void *p18=NULL,
       void *p19=NULL,
       void *p20=NULL,
       void *p21=NULL,
       void *p22=NULL,
       void *p23=NULL,
       void *p24=NULL,
       void *p25=NULL,
       void *p26=NULL,
       void *p27=NULL,
       void *p28=NULL,
       void *p29=NULL
  );


//--------------------------------------------------------------------------------------
//  Read a line from standard input
//   char s[]   : char array in which to store the line
//   lim        : maximum number of chars that can be saved in s
//-------------------------------------------------------------------------------------
int getLine( char s[], int lim);
int getLine( aString &answer );

// conversion routine used by sscanf to convert %e to %le and %f to %lf when OV_USE_DOUBLE is defined
aString ftor(const char *s);

real getCPU();

// **** here are global variables that are initialized in initOvertureGlobalVariables()****
extern const real Pi;
extern const real twoPi;

extern const aString nullString;                     // null string for default arguments
extern const aString blankString;                   // blank string for default arguments
extern const Index nullIndex;
extern const Range nullRange;
extern const Range faceRange;

int
helpOverture( const aString & documentPrefix, const aString & label );

#ifndef USE_PPP
#define GET_NUMBER_OF_ARRAYS Array_Domain_Type::getNumberOfArraysInUse()
#else
#define GET_NUMBER_OF_ARRAYS 0
#endif

#endif // WDHDEFS_H
