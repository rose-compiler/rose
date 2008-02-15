#ifndef GRID_COLLECTION_FUNCTION_H
#define GRID_COLLECTION_FUNCTION_H "GridCollectionFunction.h"

#include "doubleGridCollectionFunction.h"
#include "floatGridCollectionFunction.h"
#include "intGridCollectionFunction.h"

#include "ListOfDoubleMappedGridFunction.h"
#include "ListOfFloatMappedGridFunction.h"
#include "ListOfIntMappedGridFunction.h"

#ifdef LONGINT
  typedef longIntGridCollectionFunction IntGridCollectionFunction;
  typedef longIntGridCollectionFunction IntegerGridCollectionFunction;
#else
  typedef intGridCollectionFunction     IntGridCollectionFunction;
  typedef intGridCollectionFunction     IntegerGridCollectionFunction;
#endif // LONGINT

#ifdef OV_USE_DOUBLE
  typedef doubleGridCollectionFunction     realGridCollectionFunction;
  typedef doubleGridCollectionFunction     REALGridCollectionFunction;
  typedef doubleGridCollectionFunction     RealGridCollectionFunction;
  typedef doubleGridCollectionFunction     FloatGridCollectionFunction;
//  typedef longDoubleGridCollectionFunction DloatGridCollectionFunction;
#else
  typedef floatGridCollectionFunction  realGridCollectionFunction;
  typedef floatGridCollectionFunction  REALGridCollectionFunction;
  typedef floatGridCollectionFunction  RealGridCollectionFunction;
  typedef floatGridCollectionFunction  FloatGridCollectionFunction;
  typedef doubleGridCollectionFunction DloatGridCollectionFunction;
#endif

#endif
