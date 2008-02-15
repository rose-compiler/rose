#ifndef APP_CONVERSION_H
#define APP_CONVERSION_H

#include "A++.h"
#include "OvertureDefine.h"
// define "=" operators for mixed mode A++ operations


#define convertToIntArray convertTo_intArray

#ifdef OV_USE_DOUBLE
#define convertToRealArray convertTo_doubleArray
#else
#define convertToRealArray convertTo_floatArray
#endif

void equals(const intArray &y, const intArray & x);
void equals(const floatArray &y, const floatArray & x);
void equals(const doubleArray &y, const doubleArray & x);

void equals(const intArray &y, const floatArray & x);
void equals(const intArray &y, const doubleArray & x);
void equals(const floatArray &y, const intArray & x);
void equals(const floatArray &y, const doubleArray & x);
void equals(const doubleArray &y, const intArray & x);
void equals(const doubleArray &y, const floatArray & x);


#ifdef USE_OLD_APP
// define bitwise A++ operators
intArray  operator | ( const intArray & x, const int & value );
intArray& operator |= ( intArray & x, const int & value );
intArray  operator & ( const intArray & x, const int & value );
intArray& operator &= ( intArray & x, const int & value );
#endif

#ifdef USE_PPP
void equals(const intSerialArray &y, const intSerialArray & x);
void equals(const floatSerialArray &y, const floatSerialArray & x);
void equals(const doubleSerialArray &y, const doubleSerialArray & x);

void equals(const intSerialArray &y, const floatSerialArray & x);
void equals(const intSerialArray &y, const doubleSerialArray & x);
void equals(const floatSerialArray &y, const intSerialArray & x);
void equals(const floatSerialArray &y, const doubleSerialArray & x);
void equals(const doubleSerialArray &y, const intSerialArray & x);
void equals(const doubleSerialArray &y, const floatSerialArray & x);

#ifdef USE_OLD_APP
intSerialArray  operator | ( const intSerialArray & x, const int & value );
intSerialArray& operator |= ( intSerialArray & x, const int & value );

intSerialArray  operator & ( const intSerialArray & x, const int & value );
intSerialArray& operator &= ( intSerialArray & x, const int & value );
#endif
#endif

#endif
