#ifndef __GEOMETRIC_ADT_3D_INT_H__
#define __GEOMETRIC_ADT_3D_INT_H__

#define processedWithDT

#ifndef processedWithDT
#define GeomADTTuple3dInt GeomADTTuple2
#define GeometricADT3dInt GeometricADT3dInt
#define GeometricADTTraversor3dInt __GeometricADTtraversor2
#define GeometricADTIterator3dInt __GeometricADTiterator2


#define debug_GeometricADT3dInt debug_GeometricADT2

// define GeometricADTError GeometricADTError2
// define GeometricADTIteratorError GeometricADTIteratorError2
// define GeometricADTTraversorError GeometricADTTraversorError2
// define UnknownError UnknownError2
// define VerificationError VerificationError2
// define InvalidDepth InvalidDepth2
// define TreeInsertionError TreeInsertionError2
// define InvalidADTDimension InvalidADTDimension2
// define InvalidADTAxis InvalidADTAxis2
// define NULLADTRoot NULLADTRoot2
// define OutOfBoundingBox OutOfBoundingBox2

#define 12 (6*2)

#endif

#include <iostream.h>
#include "GeometricADTExceptions.h"
#include "NTreeNode2GeomADTTuple3dInt.h"
#ifndef OV_USE_OLD_STL_HEADERS
#include <list>
//using namespace std;
#else
#include <list.h>
#endif

#include "GeomADTTuple3dInt.h"

//typedef NTreeNode2GeomADTTuple3dInt GeometricADT3dInt::ADTType;
//typedef NTreeNode2GeomADTTuple3dInt __ADTType;
#undef __ADTType
#define __ADTType NTreeNode2GeomADTTuple3dInt



// forward declare this guy
 class GeometricADT3dInt;

// GeometricADT3dInt iterator class

class GeometricADTIterator3dInt
{
 public:
  enum LeftRightEnum
  {
    ADT_LEFT  = 0,
    ADT_RIGHT = 1
  };

  GeometricADTIterator3dInt(const GeometricADT3dInt &gADT, const real *target_);
  GeometricADTIterator3dInt(GeometricADTIterator3dInt &x);    
  ~GeometricADTIterator3dInt();
  
  GeometricADTIterator3dInt & operator= (GeometricADTIterator3dInt & i); 
  GeometricADTIterator3dInt & operator= (NTreeNode2GeomADTTuple3dInt & i);
  GeometricADTIterator3dInt & operator++();  // decend to the next node in the tree
  GeometricADTIterator3dInt  operator++(int);  // decend to the next node in the tree

  inline GeomADTTuple3dInt & operator*(); // dereference the iterator
  
  inline bool isTerminal(); // is this iterator at an end of the tree ?
  inline int getDepth();
  
  friend class GeometricADT3dInt;
  
 protected:
  // these constructors make no sense
  GeometricADTIterator3dInt();// {  } // what does this do ?
  GeometricADTIterator3dInt(GeometricADT3dInt &x);// {  }  // should probably be a standard tree iteration (eg preorder or postorder...)
  
 private:
  GeometricADT3dInt *thisADT;
  __ADTType *current;
  real target[6];
  int depth;
};


// GeometricADT3dInt traversor class

class GeometricADTTraversor3dInt  // this class should probably have a superclass in NTreeNode2GeomADTTuple3dInt
{
 public:
  enum LeftRightEnum
  {
    ADT_LEFT  = 0,
    ADT_RIGHT = 1
  };

  GeometricADTTraversor3dInt(const GeometricADT3dInt &gADT, const real *target_);
  GeometricADTTraversor3dInt(const GeometricADT3dInt &gADT);
  GeometricADTTraversor3dInt(GeometricADTTraversor3dInt &x);    
  ~GeometricADTTraversor3dInt();
  
  GeometricADTTraversor3dInt & operator= (GeometricADTTraversor3dInt & i); 
  GeometricADTTraversor3dInt & operator= (NTreeNode2GeomADTTuple3dInt & i);
  GeometricADTTraversor3dInt & operator++();  // decend to the next node in the tree
  GeometricADTTraversor3dInt  operator++(int);  // decend to the next node in the tree

  inline GeomADTTuple3dInt & operator*(); // dereference the traversor
  
  void setTarget(const real *target_);
  
  inline bool isFinished() const; // is this traversor finished traversing the tree ?

  inline int getDepth();
  
  friend class GeometricADT3dInt;

 protected:
  // these constructors make no sense
  GeometricADTTraversor3dInt() {  } // what does this do ?
  // GeometricADTTraversor3dInt(GeometricADT3dInt &x) {  } // should probably be a standard tree iteration (eg preorder or postorder...)
  inline bool isOverlapping(int depth, const real *bBox);
  inline bool isCandidate(const real *candidate);
  
 private:
  int depth;
  bool traversalFinished;

  list<bool> leftSubTreeFinished;  // keeps a stack of the traversal
  GeometricADT3dInt *thisADT;
  __ADTType *current;
  real target[6];
  real a[6];
  real b[6];
};

// Actual GeometricADT3dInt class

class GeometricADT3dInt 
{
 public:
  
  enum LeftRightEnum
  {
    ADT_LEFT  = 0,
    ADT_RIGHT = 1
  };

  GeometricADT3dInt(int rangeDimension_=2); 
  GeometricADT3dInt(int rangeDimension_, const real *boundingBox_); 

  typedef __ADTType ADTType;

  ~GeometricADT3dInt();

  typedef GeometricADTIterator3dInt  iterator;
  typedef GeometricADTTraversor3dInt traversor;
  
  void initTree();
  void initTree(int rangeDimension_, const real *boundingBox_);

  int addElement(const real *bBox, int &data);
  int delElement(GeometricADT3dInt::iterator & delItem);
  void verifyTree();


  friend class GeometricADTIterator3dInt;  
  friend class GeometricADTTraversor3dInt; 

 protected:
  inline int getSplitAxis(int depth) const;
  inline Real getSplitLocation(int axis, const real *box) const;
  void shiftTreeUp(GeometricADT3dInt::ADTType *node, int depth);  // used to rebuild the search tree
  void verifyNode(GeometricADT3dInt::ADTType &node, int depth);
  int insert(GeometricADT3dInt::iterator &insParent, int leaf, GeomADTTuple3dInt &data);
  int insert(GeometricADT3dInt::iterator &insParent, int leaf);

 private:

  int rangeDimension;
  int ADTDimension;
  int ADTdepth;
  int numberOfItems;

  real boundingBox[12];

  GeometricADT3dInt::ADTType *adtRoot;

};


//
// implementation of inlined GeometricADT3dInt methods
//



inline 
int
GeometricADT3dInt::
getSplitAxis(int depth) const 
{
  //AssertException<GeometricADT3dInt::InvalidDepth> (depth<=ADTdepth);
#if 0
  assert(depth<=ADTdepth);
#endif
  return depth%ADTDimension;
}


inline 
real 
GeometricADT3dInt::
getSplitLocation(int axis, const real *box) const
{
  //AssertException<InvalidADTDimension> (axis<ADTDimension && box.getLength(0)==2*ADTDimension);
  // assert(axis<ADTDimension && box.getLength(0)==2*ADTDimension);
  // assert(axis<ADTDimension && box.getLength(0)==2*ADTDimension);
  return (box[2*axis+1] + box[2*axis])/2.0;
}

// include "../GridGenerator/GeometricADT3dInt.C"
// include "../GridGenerator/GeometricADTIterator.C"
// include "../GridGenerator/GeometricADTTraversor.C"

//\begin{GeometricADTIterator.tex}{\subsection{Dereference operator*()}}

inline
GeomADTTuple3dInt &
GeometricADTIterator3dInt::
operator*()
// /Purpose : returns the data at the current node of the iterators {\tt GeometricADT3dInt}
//\end{GeometricADT3dInt.tex}
{
  return current->getData();
}

//\begin{GeometricADTIterator.tex}{\subsection{getDepth}}

inline
int
GeometricADTIterator3dInt::
getDepth() 
// /Purpose : returns the iterators depth in it's {\tt GeometricADT3dInt}
//\end{GeometricADT3dInt.tex}
{
  return depth;
}


inline
bool
GeometricADTTraversor3dInt::
isFinished() const
{

  return traversalFinished;

}


inline
GeomADTTuple3dInt &
GeometricADTTraversor3dInt::
operator*()
{
  return current->getData();
}


inline
int
GeometricADTTraversor3dInt::
getDepth() 
{
  return depth;
}



inline
bool 
GeometricADTTraversor3dInt::
isOverlapping(int theDepth, const real *bBox)
{
  
  // this may need to be reworked to make it more general...
  //bBox.display("bBox");
  int axis = thisADT->getSplitAxis(theDepth);
  return (a[axis]<=bBox[2*axis+1] && bBox[2*axis]<=b[axis]);

}


inline
bool 
GeometricADTTraversor3dInt::
isCandidate(const real *candidate) 
{
  for (int axis=0; axis<thisADT->ADTDimension; axis++) 
  {
    // res = res && (a[axis]<=candidate[axis] && candidate[axis]<=b[axis]);
    if( a[axis]>candidate[axis] || candidate[axis]>b[axis] )
      return false;
    //cout << " axis "<<axis<<" a "<<a(axis)<<" xk "<<candidate(axis)<<" b "<<b(axis)<<endl;
  }
  return true;
}

//\begin{>>GeometricADTIterator.tex}{\subsection{isTerminal}}

inline
bool
GeometricADTIterator3dInt::
isTerminal()
// /Purpose : return true if the iterator is a terminal leaf (ie it cannot descend any further)
//\end{GeometricADTIterator.tex}
{

  if (current==NULL) return true;

  int splitAxis = thisADT->getSplitAxis(depth);
  Real splitLoc = thisADT->getSplitLocation(splitAxis, (current->getData()).boundingBox);

  if ((current->querry(ADT_LEFT) && 
      current->querry(ADT_RIGHT)) ||
      (current->querry(ADT_LEFT) && target[splitAxis]<=splitLoc) ||
      (current->querry(ADT_RIGHT) && target[splitAxis]>splitLoc))
    return false;
  else
    return true;

}


#ifndef processedWithDT

#include "GeometricADT3dInt.C"
// include "GeometricADTIterator2.C"
// include "GeometricADTTraversor2.C"

#undef GeomADTTuple3dInt
#undef GeometricADT3dInt
#undef GeometricADTTraversor3dInt 
#undef GeometricADTIterator3dInt
#undef GeometricADTError
#undef GeometricADTIteratorError 
#undef GeometricADTTraversorError
#undef debug_GeometricADT3dInt 
#undef ADT_LEFT
#undef ADT_RIGHT
#undef UnknownError 
#undef VerificationError
#undef InvalidDepth
#undef TreeInsertionError
#undef InvalidADTDimension 
#undef InvalidADTAxis
#undef NULLADTRoot 
#undef OutOfBoundingBox
#undef 6

#endif

#endif
