#ifndef FORTRAN_IO_H
#define FORTRAN_IO_H

#include "Overture.h"


class FortranIO
{

 public:
  FortranIO();
  ~FortranIO();
  
  int open(const aString & name, 
           const aString & format,
	   const aString & status,
           const int & fortranUnitNumber=25 );
  int close();

  int print(const int & a);
  int print(const float & a);
  int print(const double & a);

  int print(const int *a, const int & count);
  int print(const float *a, const int & count);
  int print(const double *a, const int & count);

  int print(const aString & label );

  int print(const intArray & a);
  int print(const floatArray & a);
  int print(const doubleArray & a);

  int print(const intArray & a, const floatArray & b);
  int print(const intArray & a, const doubleArray & b);

  int read(const int & a);
  int read(const float & a);
  int read(const double & a);

  int read(const int *a, const int & count);
  int read(const float *a, const int & count);
  int read(const double *a, const int & count);

  int read(const aString & label );

  int read(const intArray & a);
  int read(const floatArray & a);
  int read(const doubleArray & a);

#ifdef USE_PPP
  int print(const intSerialArray & a);
  int print(const floatSerialArray & a);
  int print(const doubleSerialArray & a);

  int print(const intSerialArray & a, const floatSerialArray & b);
  int print(const intSerialArray & a, const doubleSerialArray & b);

  int read(const intSerialArray & a);
  int read(const floatSerialArray & a);
  int read(const doubleSerialArray & a);
#endif

 private:
  int io;
};

#endif
