#ifndef _RECTMDARRAY_H_
#define _RECTMDARRAY_H_
#include "Box.H"
#include <assert.h>


// RectMDArray defines rectangular data containers of a spatial dimension D
// The datatype is templated. The rectangle is defined over a Box (see Box.H)
// Thus a 3D Box with lowcorner=(0,0,0) and highcorner=(4,7,9) has 5 points along
// x dimension, 8 along y and 10 along z. The definition of Box, and therefore 
//RectMDArray is dimension independent (can be defined for any value of DIM)
 
template <class T> class RectMDArray 
{
public:
  /// Default constructor.
  RectMDArray();
  /// Constructs an RectMDArray over the Box a_box;
  RectMDArray(const Box& a_box);
  /// Defines a default-constructed RectMDArray.  
  void define(const Box& a_box);
/// Copy constructor.
  RectMDArray(const RectMDArray<T>& a_srcArray);
  /// Destructor.
  ~RectMDArray(); 
  /// Setting an RectMDArray to a constant value.
  void setVal(const T& a_val) const;
  /// Assignment operator.
  RectMDArray& operator=(const RectMDArray<T>& a_srcArray);
  /// Gets box over which array is defined.
  Box getBox() const {return m_box;};
  /// Copy on Intersection.
  void copyTo(const RectMDArray<T>& a_dest) const;
  /// Shift in place.
  void shift(const Point a_pt){m_box.shift(a_pt);};
  /// Indexing operator.
  inline T& operator[](const Point& a_iv) const;
  /// get raw pointer.
  inline T* getPointer() const;
  /// Index with integer.
  inline T& operator[](int a_index) const 
  { assert((a_index>=0) && ( a_index < m_box.sizeOf())); return m_data[a_index];}
  void print();

private:
  T* m_data;
  Box m_box;
  bool m_isDefined;
};
#include "RectMDArrayImplem.H"  
#endif
