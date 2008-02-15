#ifndef COMPOSITE_GRID_FUNCTION_H
#define COMPOSITE_GRID_FUNCTION_H "compositeGridFunction.h"

#include "doubleCompositeGridFunction.h"
#include "floatCompositeGridFunction.h"
#include "intCompositeGridFunction.h"

#ifdef LONGINT
  typedef longIntCompositeGridFunction IntCompositeGridFunction;
  typedef longIntCompositeGridFunction IntegerCompositeGridFunction;
#else
  typedef intCompositeGridFunction IntCompositeGridFunction;
  typedef intCompositeGridFunction IntegerCompositeGridFunction;
#endif // LONGINT

#ifdef OV_USE_DOUBLE
  typedef doubleCompositeGridFunction realCompositeGridFunction;
  typedef doubleCompositeGridFunction REALCompositeGridFunction;
  typedef doubleCompositeGridFunction RealCompositeGridFunction;
  typedef doubleCompositeGridFunction FloatCompositeGridFunction;
//  typedef longDoubleCompositeGridFunction DloatCompositeGridFunction;
#else
  typedef floatCompositeGridFunction  realCompositeGridFunction;
  typedef floatCompositeGridFunction  REALCompositeGridFunction;
  typedef floatCompositeGridFunction  RealCompositeGridFunction;
  typedef floatCompositeGridFunction  FloatCompositeGridFunction;
  typedef doubleCompositeGridFunction DloatCompositeGridFunction;
#endif

#endif
