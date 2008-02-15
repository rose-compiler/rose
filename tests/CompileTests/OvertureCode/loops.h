#ifndef LOOPS_H
#define LOOPS_H

#undef ForAllComponents
#define ForAllComponents( component) for (component=0; component<numberOfComponents; component++)

#undef ForAllVelocityComponents
#define ForAllVelocityComponents( component) for (component=0; component<numberOfVelocityComponents; component++)

#undef ForAxes
#define ForAxes( axis) for (axis=0; axis<numberOfDimensions; axis++)

#undef ForAllAxes
#define ForAllAxes( axis) for (axis=0; axis<3; axis++)

#undef ForAllGrids
#define ForAllGrids( grid) for (grid=0; grid<numberOfComponentGrids; grid++)

#undef ForBoundary
#define ForBoundary(side,axis)  for( axis=0; axis<numberOfDimensions; axis++) \
				for( side=0; side<=1; side++ )

//#undef inc
//#    define inc(index,direction) (index==direction ? 1 : 0)

inline int inc(const int index, const int direction) { return (index==direction ? 1 : 0);}
inline realArray Sqr (realArray & x) { return (x*x); }

#undef ndnd
#    define ndnd(i,j) i + numberOfDimensions*j
#undef n3n3
#    define n3n3(i,j) i + 3*j

#endif
