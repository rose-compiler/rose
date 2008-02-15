#ifndef DISPLAY_FUNCIONS_H
#define DISPLAY_FUNCIONS_H

#include "DisplayParameters.h"
extern const aString nullString;

int display( const    intArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );
int display( const  floatArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );
int display( const doubleArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );

int display( const    intArray & x, const char *label, const char *format_ );
int display( const  floatArray & x, const char *label, const char *format_ );
int display( const doubleArray & x, const char *label, const char *format_ );

// pass option arguments in the DisplayParameters object
int display( const    intArray & x, const char *label, const DisplayParameters & displayParameters);
int display( const  floatArray & x, const char *label, const DisplayParameters & displayParameters); 
int display( const doubleArray & x, const char *label, const DisplayParameters & displayParameters); 

// display the mask from a MappedGrid:
int displayMask( const intArray & mask, const aString & label=nullString, FILE *file = NULL);
int displayMask( const intArray & mask, const aString & label, const DisplayParameters & displayParameters);

#ifdef USE_PPP
int display( const    intSerialArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );
int display( const  floatSerialArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );
int display( const doubleSerialArray & x, const char *label= NULL, FILE *file = NULL, const char *format_=NULL );

int display( const    intSerialArray & x, const char *label, const char *format_ );
int display( const  floatSerialArray & x, const char *label, const char *format_ );
int display( const doubleSerialArray & x, const char *label, const char *format_ );

int display( const    intSerialArray & x, const char *label, const DisplayParameters & displayParameters);
int display( const  floatSerialArray & x, const char *label, const DisplayParameters & displayParameters); 
int display( const doubleSerialArray & x, const char *label, const DisplayParameters & displayParameters); 
#endif

#endif
