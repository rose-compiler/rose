#ifndef GENERIC_GRID_FUNCTION_H
#define GENERIC_GRID_FUNCTION_H "GenericGridFunction.h"

#include "doubleGenericGridFunction.h"
#include "floatGenericGridFunction.h"
#include "intGenericGridFunction.h"

#ifdef LONGINT
  typedef longIntGenericGridFunction IntGenericGridFunction;
  typedef longIntGenericGridFunction IntegerGenericGridFunction;
#else
  typedef intGenericGridFunction     IntGenericGridFunction;
  typedef intGenericGridFunction     IntegerGenericGridFunction;
#endif // LONGINT

#ifdef OV_USE_DOUBLE
  typedef doubleGenericGridFunction     realGenericGridFunction;
  typedef doubleGenericGridFunction     REALGenericGridFunction;
  typedef doubleGenericGridFunction     RealGenericGridFunction;
  typedef doubleGenericGridFunction     FloatGenericGridFunction;
//  typedef longDoubleGenericGridFunction DloatGenericGridFunction;
#else
  typedef floatGenericGridFunction  realGenericGridFunction;
  typedef floatGenericGridFunction  REALGenericGridFunction;
  typedef floatGenericGridFunction  RealGenericGridFunction;
  typedef floatGenericGridFunction  FloatGenericGridFunction;
  typedef doubleGenericGridFunction DloatGenericGridFunction;
#endif

#endif
