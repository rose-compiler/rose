#ifndef EQUI_DISTRIBUTE_H 
#define EQUI_DISTRIBUTE_H 


#include "DataPointMapping.h"
#include "GenericGraphicsInterface.h"

int
equidistribute( const realArray & w, realArray & r );
int 
equiGridSmoother(const int & domainDimension, 
		 const int & rangeDimension,
		 IntegerArray & indexRange, 
		 IntegerArray & bc, 
		 const int & axis,
		 const realArray & x, 
		 realArray & r,
                 const real arcLengthWeight=1.,
                 const real curvatureWeight=1.,
                 const real areaWeight=1.,
                 int numberOfSmooths=0 );

int 
equiGridSmoother(Mapping & map,
                 DataPointMapping & dpm, 
                 GenericGraphicsInterface & gi, 
                 GraphicsParameters & parameters,
                 IntegerArray & bc,
                 real & arcLengthWeight,
                 real & curvatureWeight,
                 real & areaWeight );

#endif
