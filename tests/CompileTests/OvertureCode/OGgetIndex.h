/*  -*-Mode: c++; -*-  */
#ifndef OG_GET_INDEX_H
#define OG_GET_INDEX_H "OGgetIndex.h"

#include "A++.h"
#include "OvertureTypes.h"
#include "wdhdefs.h"

class floatMappedGridFunction;   // forward declaration
class doubleMappedGridFunction;  // forward declaration
class intMappedGridFunction;     // forward declaration
class MappedGrid;


//=================================================================================================
//  Define some useful functions for getting A++ indicies for Overture
//
//   Note: An "index Array" is an IntegerArray of dimensions (0:1,0:2) such as indexRange, gridIndexRange,
//         or dimension.
//
//   getIndex : determine the Indices corresponding to the index space defined by an index array 
//
//   getBoundaryIndex : determine the Indices corresponding to one of the boundaries of the
//              index space defined by an index arry
//
//   getGhostIndex: This returns the indicies corresponding to a ghost line
//
//=================================================================================================

// ----------------------------getIndex---------------------------------------------------

const int OGgetIndexDefaultValue=-999999;

void 
getIndex(const IntegerArray & indexArray,   // this index array and determines I1,I2,I3
	 Index & I1,                    // output: Index for axis1
	 Index & I2,                    // output: Index for axis2
	 Index & I3,                    // output: Index for axis3
	 int extra1=0,          // increase Index's by this amount along axis1
	 int extra2=OGgetIndexDefaultValue,         // by default extra2=extra1
	 int extra3=OGgetIndexDefaultValue          // by default extra3=extra1
        );

void 
getIndex(const floatMappedGridFunction & u,   // the interior+bounnadry points of this function determine I1,I2,I3 
	 Index & I1,                          // output: Index for axis1
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,                // increase Index's by this amount along axis1
	 int extra2=OGgetIndexDefaultValue,               // by default extra2=extra1
	 int extra3=OGgetIndexDefaultValue                // by default extra3=extra1
        );

void 
getIndex(const doubleMappedGridFunction & u, 
	 Index & I1, 
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,
	 int extra2=OGgetIndexDefaultValue,
	 int extra3=OGgetIndexDefaultValue
        );

void 
getIndex(const intMappedGridFunction & u, 
	 Index & I1, 
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,
	 int extra2=OGgetIndexDefaultValue,
	 int extra3=OGgetIndexDefaultValue
        );

//-------------------------------------------------------------------------------------
// This getIndex takes a grid function and a component number and returns 3 Index's
//
// This getIndex is normally used for face centered grid functions in which case
// component indicates the value of the face-centred component (of which there can
// only be one) which was declared using the setFaceCentering member function or
// by declaring or updating a grid function with a "faceRange".
//
// For grid functions that are not face-centred, component indicates
// the value of the first component.
//-------------------------------------------------------------------------------------
void 
getIndex(const floatMappedGridFunction & u,   // return Index's for a given component
	 int component, 
	 Index & I1, 
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,
	 int extra2=OGgetIndexDefaultValue,
	 int extra3=OGgetIndexDefaultValue
        );

void 
getIndex(const doubleMappedGridFunction & u,   // return Index's for a given component
	 int component, 
	 Index & I1, 
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,
	 int extra2=OGgetIndexDefaultValue,
	 int extra3=OGgetIndexDefaultValue
        );

void 
getIndex(const intMappedGridFunction & u,   // return Index's for a given component
	 int component, 
	 Index & I1, 
	 Index & I2, 
	 Index & I3, 
	 int extra1=0,
	 int extra2=OGgetIndexDefaultValue,
	 int extra3=OGgetIndexDefaultValue
        );

// ----------------------------getBoundaryIndex---------------------------------------------------
//  These functions return Index's for a Boundary. The boundary is defined by the parameters
//  side=0,1 and axis=0,1,2.
//
//-----------------------------------------------------------------------------------------------
void 
getBoundaryIndex(const IntegerArray & indexArray,       // get Index's for boundary of an indexArray
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );

void 
getBoundaryIndex(const floatMappedGridFunction & u,  // get Index's for boundary
                 int component,
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );
void 
getBoundaryIndex(const doubleMappedGridFunction & u, 
                 int component,
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );
void 
getBoundaryIndex(const intMappedGridFunction & u, 
                 int component,
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );
void 
getBoundaryIndex(const floatMappedGridFunction & u,    // get Index's for boundary, component=0
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );
void 
getBoundaryIndex(const doubleMappedGridFunction & u,  // get Index's for boundary, component=0
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );
void 
getBoundaryIndex(const intMappedGridFunction & u,    // get Index's for boundary, component=0
		 int side, 
		 int axis, 
		 Index & Ib1, 
		 Index & Ib2, 
		 Index & Ib3, 
		 int extra1=0,
		 int extra2=OGgetIndexDefaultValue,
		 int extra3=OGgetIndexDefaultValue
		 );

//-----------------------------------------------------------------------------------------------
// These functions return the Index's for a Ghost-line. These are similar to getBoundaryIndex
// and in fact will give the same answer as getBoundaryIndex for the choice ghostLine=0.
// 
//  Input:
//    side,axis : get Index's for this side and axis
//    ghostLine : get Index's for this ghost-line, 
//                   ghostLine=1 : first ghost-line
//                   ghostLine=2 : second ghost-line
//                   ghostLine=0 : boundary
//                   ghostLine=-1: first line inside
//----------------------------------------------------------------------------------------------
void 
getGhostIndex(const IntegerArray & indexArray,          // get Index's for ghost line
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2, 
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );

void 
getGhostIndex(const floatMappedGridFunction & u,   // get Index's for ghost line for a grid function
	      int component,
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );
void 
getGhostIndex(const doubleMappedGridFunction & u,   // get Index's for ghost line for a grid function
	      int component,
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );
void 
getGhostIndex(const intMappedGridFunction & u,   // get Index's for ghost line for a grid function
	      int component,
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );
void 
getGhostIndex(const floatMappedGridFunction & u,   // get Index's for ghost line for a grid function, component=0
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );
void 
getGhostIndex(const doubleMappedGridFunction & u,   // get Index's for ghost line for a grid function, component=0
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
        );
void 
getGhostIndex(const intMappedGridFunction & u,   // get Index's for ghost line for a grid function, component=0
	      int side, 
	      int axis,
	      Index & Ib1, 
	      Index & Ib2,
	      Index & Ib3, 
	      int ghostLine=1, 
	      int extra1=0,
	      int extra2=OGgetIndexDefaultValue,
	      int extra3=OGgetIndexDefaultValue
	      );

IntegerArray
extendedGridIndexRange(const MappedGrid & mg);  // gridIndexRange extended for interpolation boundaries

IntegerArray
extendedGridRange(const MappedGrid & mg);  // gridIndexRange extended for interpolation and mixed  boundaries


#endif
