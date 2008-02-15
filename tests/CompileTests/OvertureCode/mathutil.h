// Math utilities
#ifndef MATHUTIL_H
#define MATHUTIL_H "Mathutil.h"

#define SQR(x) ((x)*(x))
#define SQRT(x) pow((x),.5)

#include "OvertureTypes.h"  // define real to be float or double

inline int max(const int& x1,const int& x2 ){ return x1>x2 ? x1 : x2; }
inline double max(const int& x1,const double& x2 ){ return x1>x2 ? x1 : x2; }
inline double max(const double& x1,const int& x2 ){ return x1>x2 ? x1 : x2; }
inline float max(const int& x1,const float& x2 ){ return x1>x2 ? x1 : x2; }
inline float max(const float& x1,const int& x2 ){ return x1>x2 ? x1 : x2; }

inline float max(const float& x1,const float& x2 ){ return x1>x2 ? x1 : x2; }

inline double max(const double& x1,const double& x2 ){ return x1>x2 ? x1 : x2; }

inline double max(const float& x1,const double& x2 ){ return x1>x2 ? double(x1) : x2; }

inline double max(const double& x1,const float& x2 ){ return x1>x2 ? x1 : double(x2); }

// ifndef USE_PPP *no longer needed* 010117
inline int min(const int& x1,const int& x2 ){ return x1<x2 ? x1 : x2; }
// endif

inline float min(const float& x1,const float& x2 ){ return x1<x2 ? x1 : x2; }

inline double min(const double& x1,const double& x2 ){ return x1<x2 ? x1 : x2; }

inline double min(const float& x1,const double& x2 ){ return x1<x2 ? double(x1) : x2; }

inline double min(const double& x1,const float& x2 ){ return x1<x2 ? x1 : double(x2); }

// ************************************************
// **** here are max/min for multiple arguments ***
// ************************************************
inline int max(const int& x1,const int& x2,const int& x3 ){ return max(x1,max(x2,x3)); }
inline int max(const int& x1,const int& x2,const int& x3,const int& x4 ){ return max(x1,max(x2,x3,x4));}
inline int max(const int& x1,const int& x2,const int& x3,const int& x4,const int& x5 )
   { return max(x1,max(x2,x3,x4,x5));}

inline float max(const float& x1,const float& x2,const float& x3 ){ return max(x1,max(x2,x3)); }
inline float max(const float& x1,const float& x2,const float& x3,const float& x4 )
 { return max(x1,max(x2,x3,x4));}
inline float max(const float& x1,const float& x2,const float& x3,const float& x4,const float& x5 )
 { return max(x1,max(x2,x3,x4,x5));}

inline double max(const double& x1,const double& x2,const double& x3 ){ return max(x1,max(x2,x3)); }
inline double max(const double& x1,const double& x2,const double& x3,const double& x4 )
 { return max(x1,max(x2,x3,x4));}
inline double max(const double& x1,const double& x2,const double& x3,const double& x4,const double& x5 )
 { return max(x1,max(x2,x3,x4,x5));}

inline int min(const int& x1,const int& x2,const int& x3 ){ return min(x1,min(x2,x3)); }
inline int min(const int& x1,const int& x2,const int& x3,const int& x4 ){ return min(x1,min(x2,x3,x4));}
inline int min(const int& x1,const int& x2,const int& x3,const int& x4,const int& x5 )
   { return min(x1,min(x2,x3,x4,x5));}

inline float min(const float& x1,const float& x2,const float& x3 ){ return min(x1,min(x2,x3)); }
inline float min(const float& x1,const float& x2,const float& x3,const float& x4 )
 { return min(x1,min(x2,x3,x4));}
inline float min(const float& x1,const float& x2,const float& x3,const float& x4,const float& x5 )
 { return min(x1,min(x2,x3,x4,x5));}

inline double min(const double& x1,const double& x2,const double& x3 ){ return min(x1,min(x2,x3)); }
inline double min(const double& x1,const double& x2,const double& x3,const double& x4 )
 { return min(x1,min(x2,x3,x4));}
inline double min(const double& x1,const double& x2,const double& x3,const double& x4,const double& x5 )
 { return min(x1,min(x2,x3,x4,x5));}


// these are needed by the dec compiler
inline float pow(const float& x1,const int& x2 ){ return x2==0 ? 1 : pow(double(x1),double(x2)); }
#ifndef __hpux
inline double pow(const double& x1,const int& x2 ){ return x2==0 ? 1 : pow(x1,double(x2)); }
#endif
inline double pow(const float& x1,const double& x2 ){ return pow(double(x1),x2); }
inline double pow(const double& x1,const float& x2 ){ return pow(x1,double(x2)); }
inline float pow(const float& x1,const float& x2 ){ return pow(double(x1),double(x2)); }
inline int round(const double& x ){ return x>0 ? int(x+.5) : int(x-.5); } //  round to nearest integer
inline double pow(const int& x1,const int& x2 ){ return x2>0 ? round(pow(double(x1),double(x2))) : pow(double(x1),double(x2)); } 
#ifndef __KCC
inline float atan2( float  x1, float  x2 ){ return atan2(double(x1),double(x2)); } 
#endif
inline float atan2(const double& x1, float  x2 ){ return atan2(double(x1),double(x2)); } 
inline float atan2( float  x1,const double& x2 ){ return atan2(double(x1),double(x2)); } 
inline float fmod( float  x1, float  x2 ){ return fmod(double(x1),double(x2)); } 
inline float fmod(const double& x1, float  x2 ){ return fmod(double(x1),double(x2)); } 
inline float fmod( float  x1,const double& x2 ){ return fmod(double(x1),double(x2)); } 
  
inline int sign(const int    & x ) { return x>0 ? 1 : ( x<0 ? -1 : 0);}
inline int sign(const float  & x ) { return x>0 ? 1 : ( x<0 ? -1 : 0);}
inline int sign(const double & x ) { return x>0 ? 1 : ( x<0 ? -1 : 0);}

inline double log10( const int & x ){ return log10((double)x); }
inline double log( const int & x ){ return log((double)x); }

inline double fabs( const int & x ){ return fabs((double)x); }

int inline 
floorDiv(int numer, int denom )
//  return the floor( "numer/denom" ) (ie. always chop to the left).
//  Assumes denom>0
//  
//  floorDiv(  3,2)  = 1  same as 3/2
//  floorDiv( -3,2 ) =-2  **note** not the same as (-3)/2 = -1
{
  if( numer>0 )
    return numer/denom;
  else
    return (numer-denom+1)/denom;
}
   

#endif // MATHUTIL_H
