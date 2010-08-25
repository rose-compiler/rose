#ifndef SPARSE_REP_H
#define SPARSE_REP_H

#include "GridCollectionFunction.h"
#include "MappedGridFunction.h"


// class used to save information about the sparse representation of coefficient MappedGridFunctions


// Notes:
//   Equations are numbered 1,2,3,4,...
//
//   For multi component equations such as
//        \Delta u + f_1(u,v) = 0
//         \Delta v + f_2(u,v) = 0
// The variables are numbered
//     [ u_1, v_1, u_2, v_2, u_3, v_3, .... ]
//        1    2    3    4    5    6 ....


class SparseRepForMGF : public ReferenceCounting
{
 // protected:
public:  // make public for now
 
  intMappedGridFunction equationNumber;  // eqn number for each coefficient
  intMappedGridFunction classify;        // classify each point
  
  int equationOffset;   // offset for equation number (when we are really dealing with multiple grids)
  int numberOfComponents;  // size of the system of equations
  int numberOfGhostLines;
  int stencilSize;

public:
  enum classifyTypes  // any non-negative value indicates a used point. Negative values
  {                   // are equations with zero for the rhs
    interior=1,
    boundary=2,
    ghost1=3,
    ghost2=4,
    ghost3=5,
    ghost4=6,
    interpolation=-1,
    periodic=-2,
    extrapolation=-3,
    unused=0
  };

  SparseRepForMGF();
  ~SparseRepForMGF();
  SparseRepForMGF(const SparseRepForMGF & rep);                // copy constructor
  SparseRepForMGF & operator=( const SparseRepForMGF & rep );  // deep copy
  ReferenceCounting& operator=(const ReferenceCounting& x){ if (&x); return *this; }
  
  int updateToMatchGrid(MappedGrid & mg, 
			int stencilSize=9,
			int numberOfGhostLines=1,
			int numberOfComponents=1, 
                        int offset=0);

  virtual void setParameters(int stencilSize=9,        
                             int numberOfGhostLines=1, 
			     int numberOfComponents=1, 
			     int offset=0 );

  virtual int setCoefficientIndex(const int  & m, 
                      const int & na, const Index & I1a, const Index & I2a, const Index & I3a,
                      const int & nb, const Index & I1b, const Index & I2b, const Index & I3b);

  virtual int setCoefficientIndex(const int  & m, 
                      const int & na, const Index & I1a, const Index & I2a, const Index & I3a,
                      const int & equationNumber );

  // assign the classify at given points
  virtual int setClassify(const classifyTypes & type, 
                          const Index & I1, const Index & I2, const Index & I3, const Index & N=nullIndex);

  // fixup up the classify array to take into account the mask array and periodicity
  virtual int fixUpClassify( realMappedGridFunction & coeff );

  virtual int indexToEquation( int n, int i1, int i2, int i3);

  virtual int equationToIndex( const int eqnNo, int & n, int & i1, int & i2, int & i3 );
  
  // return size of this object  
  virtual real sizeOf(FILE *file = NULL ) const;

public:
  static int debug;
};

/* ------


// class used to save information about the sparse representation of coefficient GridCollectionFunctions
class SparseRepForGCF
{
protected:
  intMappedGridFunction equationNumber;
  inline int indexToEquation( int n, int i1, int i2, int i3, int grid);
  int equationToIndex( const int eqnNo, int & n, int & i1, int & i2, int & i3, int & grid );
  
public:
  SparseRepForGCF();
  ~SparseRepForGCF();
  
  int updateToMatchGrid(GridCollection & gc, int stencilSize=9, int numberOfComponents=1);
  virtual int setCoefficientIndex(const Index & M, 
                      const Index & Na, const Index & I1a, const Index & I2a, const Index & I3a, int grida,
                      const Index & Nb, const Index & I1b, const Index & I2b, const Index & I3b, int gridb);
  
};



int SparseRepForGCF::
indexToEquation( int n, int i1, int i2, int i3, int grid)
//=============================================================================
// Return the equation number for given indices
//  n : component number ( n=0,1,..,numberOfComponents-1 )
//  i1,i2,i3 : grid indices
//  grid : component grid number (grid=0,1,2..,numberOfComponentGrids-1)   
//
//=============================================================================
{
  // return list[grid].indexToEquation(n,i1,i2,i3);
}

----- */
#endif
