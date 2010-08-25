#ifndef DISPLAY_PARAMETERS_H
#define DISPLAY_PARAMETERS_H

#include "A++.h"
#include "aString.H"

class DisplayParameters
{
 public:
  enum DisplayOption
  {
    intFormat,               // set format for int's  
    floatFormat,             // set format for float's
    doubleFormat,            // set format for double's
    labelAllIndicies,        // put comments to number all indicies
    labelNoIndicies,         // no comments
    labelIndex,              // indicate which indicies to label or not.
    forwardOrdering,         // print index 0, then index 1, ...
    backwardOrdering,        // print last index first, second-last next, ...
    strideForAllIndicies,    // print a specified index as base,base+stride,... (if stride>0) 
    indexStride              //   or bound,bound-stride,...(if stride<0)
  };
  
  DisplayParameters();
  ~DisplayParameters();

  int set(FILE *file);
  int set(const DisplayOption & displayOption);
  int set(const DisplayOption & displayOption, const int & value );
  int set(const DisplayOption & displayOption, const aString & value );
  int set(const DisplayOption & displayOption, const int & index, const int & value );
  
 protected:
  FILE *file;
  aString iFormat,fFormat,dFormat;  // formats for int, float, double
  intArray indexLabel;
  bool ordering;
  intArray stride;

  friend class intGridCollectionFunction;
  friend class floatGridCollectionFunction;
  friend class doubleGridCollectionFunction;
  friend int display( const intArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );
  
  friend int display( const floatArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );
  
  friend int display( const doubleArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );

  friend int displayMaskPrivate( const intArray & mask, 
                                 const aString & label,
				 FILE *file,
				 const DisplayParameters *displayParameters );

#ifdef USE_PPP
  friend int display( const intSerialArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );
  
  friend int display( const floatSerialArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );
  
  friend int display( const doubleSerialArray & x, 
		      const char *label,
		      FILE *file,
		      const char *format_ ,
		      const DisplayParameters *displayParameters );
#endif
};

#endif
