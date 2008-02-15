#ifndef PARALLEL_UTILITY_H
#define PARALLEL_UTILITY_H

#include "Overture.h"

// broadcast an int to all processors
void broadCast( int & value, const int & fromProcessor=0 );

// broadcast a serial array to all processors
void broadCast( intSerialArray & buff, const int & fromProcessor=0 );

// broadcast a aString to all processors
void broadCast( aString & string, const int & fromProcessor=0 );


// redistribute a grid and grid function to a new set of processors
void redistribute(const realGridCollectionFunction & u, 
		  GridCollection & gcP,
		  realGridCollectionFunction & v, 
		  const Range & Processors );

// redistribute a grid and grid function to a new set of processors
void redistribute(const GridCollection & gc,
		  GridCollection & gcP,
		  const Range & Processors );

#endif
