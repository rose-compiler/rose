#ifndef TRIANGLE_H 
#define TRIANGLE_H 

#include "Mapping.h"

class Triangle
{
 public:
  Triangle();
  Triangle( const real x1[3], const real x2[3], const real x3[3] );
  Triangle( const RealArray & x1, const RealArray & x2, const RealArray & x3 );
  Triangle( const realArray & grid,
	    const int & i1,
	    const int & i2,
	    const int & i3, 
	    const int & choice=0,
	    const int & axis=axis3 );
  ~Triangle();
  
  real area() const; // return the area of the triangle
  void setVertices( const real x1_[3], const real x2_[3], const real x3_[3] );
  void setVertices( const RealArray & x1, const RealArray & x2, const RealArray & x3 );
  void setVertices(const realArray & grid, 
                   const int & i1, 
		   const int & i2, 
		   const int & i3, 
		   const int & choice=0,
		   const int & axis=axis3 );

  // does this triangle intersect another? return segment of intersection
  bool intersects(Triangle & triangle, real xi1[3], real xi2[3] ) const; 
  bool intersects(Triangle & triangle, RealArray & xi1, RealArray & xi2 ) const; 
  
  // does a ray from the point x to y=infinity cross the triangle?
  bool intersects(real x[3], real xi[3] ) const;
  bool intersects(RealArray & x, RealArray &  xi ) const;

  int getRelativeCoordinates( const real x[3], real & alpha1, real & alpha2, const bool & shouldBeInside=TRUE ) const;

  void display(const aString & label=blankString) const;

  double tetraheadralVolume(const real a[], const real b[], const real c[], const real d[]) const;

  real x1[3], x2[3], x3[3]; // three vertices
  real normal[4];   // holds normal plus constant for eqn of the plane, n.x+c=0
  void computeNormal();

};

#endif
