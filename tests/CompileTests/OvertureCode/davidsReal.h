/*  -*-Mode: c++; -*-  */
#ifndef _DavidsReal_H_
#define _DavidsReal_H_  "REAL.h"

#include "OvertureDefine.h"
#ifndef OV_USE_DOUBLE
#     define REAL float
#     define REALArray floatArray
#     define REALStreamArray floatStreamArray
#     define ListOfREALArray ListOffloatArray
#     define REALCompositeGridFunction floatCompositeGridFunction
#     define REALMappedGridFunction floatMappedGridFunction
#else
#     define REAL double
#     define REALArray doubleArray
#     define REALStreamArray doubleStreamArray
#     define ListOfREALArray ListOfdoubleArray
#     define REALCompositeGridFunction doubleCompositeGridFunction
#     define REALMappedGridFunction doubleMappedGridFunction
//#else
//ERROR:: REAL.H must define USE_FLOAT or USE_DOUBLE
#endif

#endif
