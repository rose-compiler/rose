#ifndef __KKC_GEOMETRIC_SEARCH__
#define __KKC_GEOMETRIC_SEARCH__

#include <iostream.h>
#include "Overture.h"
#include "NTreeNode.h"
#include "GeometricADTExceptions.h"
#include "AssertException.h"
#ifndef OV_USE_OLD_STL_HEADERS
#include <list>
//using namespace std;
#else
#include <list.h>
#endif

template<class dataT>
class GeomADTTuple // helper class to be used in the GeometricADT
{
 public:
  GeomADTTuple() { ; }
  GeomADTTuple(GeomADTTuple &x) 
    {
      boundingBox = evaluate(x.boundingBox);
      data = x.data;
    }
  GeomADTTuple(realArray & boundingBox_, realArray &coords_, dataT i) 
    {
      this->boundingBox = evaluate(boundingBox_);
      data = i;
      this->coords = evaluate(coords_);
    }
  GeomADTTuple & operator=(GeomADTTuple &x) 
    {
      boundingBox = evaluate(x.boundingBox);
      data = x.data;
      coords = evaluate(x.coords);
      return *this;
    }
  ~GeomADTTuple() { ; }
  realArray boundingBox;
  realArray coords;
  dataT data;
};

//typedef NTreeNode<2,GeomADTTuple> GeometricADT<dataT>::ADTType;
//typedef NTreeNode<2,GeomADTTuple<dataT> > __ADTType;
#define __ADTType NTreeNode<2,GeomADTTuple<dataT> >

const int ADT_LEFT  = 0;
const int ADT_RIGHT = 1;


// forward declare this guy
template<class dataT> class GeometricADT;

// GeometricADT iterator class
template<class dataT>
class __GeometricADTiterator
{
 public:
  __GeometricADTiterator(const GeometricADT<dataT> &gADT, realArray & target_);
  __GeometricADTiterator(__GeometricADTiterator &x);    
  ~__GeometricADTiterator();
  
  __GeometricADTiterator & operator= (__GeometricADTiterator<dataT> & i); 
  __GeometricADTiterator & operator= (NTreeNode<2,GeomADTTuple<dataT> > & i);
  __GeometricADTiterator & operator++();  // decend to the next node in the tree
  __GeometricADTiterator  operator++(int);  // decend to the next node in the tree

  GeomADTTuple<dataT> & operator*(); // dereference the iterator
  
  bool isTerminal(); // is this iterator at an end of the tree ?
  int getDepth();
  
  friend class GeometricADT<dataT>;
  
 protected:
  // these constructors make no sense
  __GeometricADTiterator();// {  } // what does this do ?
  __GeometricADTiterator(GeometricADT<dataT> &x);// {  }  // should probably be a standard tree iteration (eg preorder or postorder...)
  
 private:
  GeometricADT<dataT> *thisADT;
  __ADTType *current;
  realArray target;
  int depth;
};


// GeometricADT traversor class
template<class dataT>
class __GeometricADTtraversor  // this class should probably have a superclass in NTreeNode
{
 public:
  __GeometricADTtraversor(const GeometricADT<dataT> &gADT, realArray & target_);
  __GeometricADTtraversor(__GeometricADTtraversor &x);    
  ~__GeometricADTtraversor();
  
  __GeometricADTtraversor & operator= (__GeometricADTtraversor<dataT> & i); 
  __GeometricADTtraversor & operator= (NTreeNode<2,GeomADTTuple<dataT> > & i);
  __GeometricADTtraversor & operator++();  // decend to the next node in the tree
  __GeometricADTtraversor  operator++(int);  // decend to the next node in the tree

  GeomADTTuple<dataT> & operator*(); // dereference the traversor
  
  bool isFinished() const; // is this traversor finished traversing the tree ?

  int getDepth();
  
  friend class GeometricADT<dataT>;

 protected:
  // these constructors make no sense
  __GeometricADTtraversor() {  } // what does this do ?
  __GeometricADTtraversor(GeometricADT<dataT> &x) {  } // should probably be a standard tree iteration (eg preorder or postorder...)
  bool isOverlapping(int depth, const realArray &bBox);
  bool isCandidate(const realArray &candidate);
  
 private:
  int depth;
  bool traversalFinished;

  list<bool> leftSubTreeFinished;  // keeps a stack of the traversal
  GeometricADT<dataT> *thisADT;
  __ADTType *current;
  realArray target;
  realArray a;
  realArray b;
};

// Actual GeometricADT class
template<class dataT>
class GeometricADT 
{
 public:
  
  GeometricADT(int rangeDimension_=2); 
  GeometricADT(const realArray & boundingBox_); 

  typedef __ADTType ADTType;

  ~GeometricADT();

  typedef __GeometricADTiterator<dataT>  iterator;
  typedef __GeometricADTtraversor<dataT> traversor;
  
  void initTree();
  void initTree(const realArray & boundingBox_);
  int addElement(realArray & bBox, dataT &data);
  int delElement(GeometricADT<dataT>::iterator & delItem);
  void verifyTree();

  const realArray &getBoundingBox() const { return boundingBox; } // probably should just return a copy of 
                                                                  // the boundingBox ? 
  friend class __GeometricADTiterator<dataT>;  
  friend class __GeometricADTtraversor<dataT>; 

 protected:
  inline int getSplitAxis(int depth) const;
  inline Real getSplitLocation(int axis, const realArray & box) const;
  void shiftTreeUp(GeometricADT<dataT>::ADTType *node, int depth);  // used to rebuild the search tree
  void verifyNode(GeometricADT<dataT>::ADTType &node, int depth);
  int insert(GeometricADT<dataT>::iterator &insParent, int leaf, GeomADTTuple<dataT> &data);

 private:

  int rangeDimension;
  int ADTDimension;
  int ADTdepth;
  int numberOfItems;

  realArray boundingBox;

  GeometricADT<dataT>::ADTType *adtRoot;

};


//
// implementation of inlined GeometricADT methods
//


template<class dataT>
inline 
int
GeometricADT<dataT>::
getSplitAxis(int depth) const 
{
  //AssertException<GeometricADT::InvalidDepth> (depth<=ADTdepth);
  assert(depth<=ADTdepth);
  return depth%ADTDimension;
}

template<class dataT>
inline 
real 
GeometricADT<dataT>::
getSplitLocation(int axis, const realArray & box) const
{
  //AssertException<InvalidADTDimension> (axis<ADTDimension && box.getLength(0)==2*ADTDimension);
  assert(axis<ADTDimension && box.getLength(0)==2*ADTDimension);
  return (box(2*axis+1) + box(2*axis))/2.0;
}

#include "../GridGenerator/GeometricADT.C"
#include "../GridGenerator/GeometricADTIterator.C"
#include "../GridGenerator/GeometricADTTraversor.C"


#endif
