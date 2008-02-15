#ifndef MAPPED_GRID_FUNCTION_H
#define MAPPED_GRID_FUNCTION_H "MappedGridFunction.h"

#include "doubleMappedGridFunction.h"
#include "floatMappedGridFunction.h"
#include "intMappedGridFunction.h"

#ifdef LONGINT
  typedef longIntMappedGridFunction IntMappedGridFunction;
  typedef longIntMappedGridFunction IntegerMappedGridFunction;
#else
  typedef intMappedGridFunction     IntMappedGridFunction;
  typedef intMappedGridFunction     IntegerMappedGridFunction;
#endif // LONGINT

#ifdef OV_USE_DOUBLE
  typedef doubleMappedGridFunction     realMappedGridFunction;
  typedef doubleMappedGridFunction     REALMappedGridFunction;
  typedef doubleMappedGridFunction     RealMappedGridFunction;
  typedef doubleMappedGridFunction     FloatMappedGridFunction;
//  typedef longDoubleMappedGridFunction DloatMappedGridFunction;
#else
  typedef floatMappedGridFunction  realMappedGridFunction;
  typedef floatMappedGridFunction  REALMappedGridFunction;
  typedef floatMappedGridFunction  RealMappedGridFunction;
  typedef floatMappedGridFunction  FloatMappedGridFunction;
  typedef doubleMappedGridFunction DloatMappedGridFunction;
#endif

#include "MappedGrid.h"

#endif
