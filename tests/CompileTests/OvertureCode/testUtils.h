/*  -*-Mode: c++; -*-  */
#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include "davidsReal.h"
#include "Projection.h"
#include "OGFunction.h"
#include "Overture.h"

void 
printMaxNormOfScalar (realCompositeGridFunction & scalar, CompositeGrid & cg);

real 
getMaxNormOfScalarOnAllGrids (realCompositeGridFunction & scalar, CompositeGrid & cg);

real
getMaxNormOfScalarOnGrid (realMappedGridFunction & scalar, MappedGrid & mg);

void 
printMaxNormOfProjectedDivergence (realCompositeGridFunction & divergence, CompositeGrid & cg);

void 
printMaxNormOfVelocity (realCompositeGridFunction & velocity, CompositeGrid & cg);

void 
printMaxNormOfDifference (const realMappedGridFunction & u, const realMappedGridFunction & v);

void 
printMaxNormOfDifference (const realGridCollectionFunction & u, const realGridCollectionFunction & v);

void 
printMaxNormOfGridFunction (realMappedGridFunction & mgf, MappedGrid & mg);

void 
applyExactBoundaryConditions (realCompositeGridFunction & projectedVelocity, 
			      CompositeGrid & cg,
			      Projection & projection);

Projection::ExactVelocityType 
chooseExactVelocityType ();

void
setProjectionEllipticSolverParameters (Projection& projection);

bool 
setBoolParameter (const aString & label);

float
setFloatParameter (const aString & label);

int 
setIntParameter (const aString & label);

bool
requestGrid (CompositeGrid& cg, int& numberOfGrids, int& numberOfDimensions);


#endif
