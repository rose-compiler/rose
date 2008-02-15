#ifndef LIST_OF_REAL_ARRAY_H
#define LIST_OF_REAL_ARRAY_H

#ifdef OV_USE_DOUBLE
#include "ListOfDoubleSerialArray.h"
#include "ListOfDoubleDistributedArray.h"
typedef ListOfDoubleSerialArray ListOfRealArray;
typedef ListOfDoubleDistributedArray ListOfRealDistributedArray;
#else
#include "ListOfFloatSerialArray.h"
#include "ListOfFloatDistributedArray.h"
typedef ListOfFloatSerialArray ListOfRealArray;
typedef ListOfFloatDistributedArray ListOfRealDistributedArray;
#endif

#endif
