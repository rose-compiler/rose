#ifndef MAPPED_GRID_OPERATORS_INCLUDE_H
#define MAPPED_GRID_OPERATORS_INCLUDE_H

// define some stuff used internally by the MGOperators


// This next macro is used to optionally use a where mask or not, depending
//  on whether any unused points or interiorBoundaryPoints on the grid. The user
// may also decide to optionally apply a boundary condition at only some points
// using the mask in the BoundaryConditionParameters.
//  *** NOTE *** 
//    --> We assume that a PDE solver will interpolate and then apply BC's
//    --> We DO apply boundary conditions at normal interpolation points, over-writing
//        values assigned by the interpolator
//    --> We DO NOT apply boundary conditions at interiorBoundaryPoints since we should
//        not over-write values put there by the interpolator.
#define USE_MASK_IN_OPERATORS
#ifdef USE_MASK_IN_OPERATORS

#define WHERE_MASK(EXPRESSION) \
          if( useWhereMaskOnBoundary[axis][side] || bcParameters.getUseMask() ) \
          { \
	    where( mask ) \
            {\
	      EXPRESSION \
	    }  \
          }   \
          else \
            {\
	      EXPRESSION \
	    }

#define WHERE_MASK_ALL_COMPONENTS(EXPRESSION) \
          if( useWhereMaskOnBoundary[axis][side] || bcParameters.getUseMask() ) \
          { \
	    where( mask ) \
            {\
               for( n=uC.getBase(0); n<=uC.getBound(0); n++ ) \
  	       { \
 	         EXPRESSION \
  	       }  \
            }  \
          }   \
          else \
            {\
               for( n=uC.getBase(0); n<=uC.getBound(0); n++ ) \
  	       { \
 	         EXPRESSION \
  	       }  \
            }


// This is just like one above except that mask has a first dimension of length 1
#define WHERE_MASK0(EXPRESSION) \
          if( useWhereMaskOnBoundary[axis][side] || bcParameters.getUseMask()  ) \
           { \
            where( mask(0,I1m,I2m,I3m) ) \
            {\
	      EXPRESSION \
	    }  \
          }   \
          else \
            {\
	      EXPRESSION \
	    }
#else
#define WHERE_MASK
#define WHERE_MASK0
#endif

#endif
