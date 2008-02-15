#ifndef GENERIC_GRID_COLLECTION_FUNCTION_H
#define GENERIC_GRID_COLLECTION_FUNCTION_H "GenericGridCollectionFunction.h"

#include "doubleGenericGridCollectionFunction.h"
#include "floatGenericGridCollectionFunction.h"
#include "intGenericGridCollectionFunction.h"

#include "ListOfDoubleGenericGridFunction.h"
#include "ListOfFloatGenericGridFunction.h"
#include "ListOfIntGenericGridFunction.h"

#ifdef LONGINT
  typedef longIntGenericGridCollectionFunction IntGenericGridCollectionFunction;
  typedef longIntGenericGridCollectionFunction IntegerGenericGridCollectionFunction;
#else
  typedef intGenericGridCollectionFunction     IntGenericGridCollectionFunction;
  typedef intGenericGridCollectionFunction     IntegerGenericGridCollectionFunction;
#endif // LONGINT

#ifdef OV_USE_DOUBLE
  typedef doubleGenericGridCollectionFunction     realGenericGridCollectionFunction;
  typedef doubleGenericGridCollectionFunction     REALGenericGridCollectionFunction;
  typedef doubleGenericGridCollectionFunction     RealGenericGridCollectionFunction;
  typedef doubleGenericGridCollectionFunction     FloatGenericGridCollectionFunction;
//  typedef longDoubleGenericGridCollectionFunction DloatGenericGridCollectionFunction;
#else
  typedef floatGenericGridCollectionFunction  realGenericGridCollectionFunction;
  typedef floatGenericGridCollectionFunction  REALGenericGridCollectionFunction;
  typedef floatGenericGridCollectionFunction  RealGenericGridCollectionFunction;
  typedef floatGenericGridCollectionFunction  FloatGenericGridCollectionFunction;
  typedef doubleGenericGridCollectionFunction DloatGenericGridCollectionFunction;
#endif

#endif
