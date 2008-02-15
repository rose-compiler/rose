#ifndef BOUNDING_BOX_H 
#define BOUNDING_BOX_H  "BoundingBox.h"

#include "A++.h"
#include "OvertureTypes.h"
#include "wdhdefs.h"
#include "aString.H"            // Livermore aString Library
// include "tlist.h"

class ListOfBoundingBox;      // forward declaration

// This class defines a bounding box for a portion of a grid. This
// bounding box is normally put into a binary tree so it has
// pointers to a parent and two children.
// 
//   domain : is index space, domain bounds are int's
//   range : is cartesian space, range bounds are real's

class BoundingBox 
{
public:
  BoundingBox *parent;
  BoundingBox *child1;
  BoundingBox *child2;


  BoundingBox( const int domainDimension=3, const int rangeDimension=3 );
  ~BoundingBox();

  inline int& domainBound(int side,int axis) { return domain[side][axis]; }
  inline real& rangeBound(int side,int axis) { return range[side][axis]; }

  inline int domainBound(int side,int axis) const { return domain[side][axis]; }
  inline real rangeBound(int side,int axis) const { return range[side][axis]; }

  // return the domain bounds
  IntegerArray getDomainBound() const;
  RealArray getRangeBound() const;
  
  int setDomainBound( const IntegerArray & db);
  int setRangeBound( const RealArray & rb);

  int domainBoundEquals(const BoundingBox & b);
  int rangeBoundEquals(const BoundingBox & b);

  inline bool isDefined() const {return defined;}
  // It is up to the application to specify when the box is defined. The box is
  // not defined when initially created.
  void setIsDefined() {defined=TRUE;}

  void setDimensions( const int domainDimension=3, const int rangeDimension=3 );
  void addChildren();
  void deleteChildren();
  void display( const aString & comment=nullString ) const;

  // does this box intersect another (in the range space).
  bool intersects( const BoundingBox & box ) const;  

 private:
  int domainDimension;
  int rangeDimension;


  int defined;          // true if domainBound and rangeBound have been assigned
  int domain[2][3];
  real range[2][3];
};


//
// This class holds a simple LIFO stack of bounding boxes.
//
class BoundingBoxStack
{
 public:
  BoundingBoxStack();

  ~BoundingBoxStack();

  void push( BoundingBox & bb );

  BoundingBox& pop();

  int isEmpty() const;

 private:
  int bottom,top;
  ListOfBoundingBox *stack;

};


#endif
