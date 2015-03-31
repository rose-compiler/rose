#ifndef _RECTMDARRAY_H_
#define _RECTMDARRAY_H_
#include "Box.H"
#include <memory>
#include <cassert>
#include <functional>

class Interval 
{
public:
  unsigned int low, high;
  Interval(const unsigned int & a_low, 
           const unsigned int & a_high)
  {
    low  = a_low;
    high = a_high;
  }
};

template<class T, unsigned int C, unsigned char... RestD>
class Tensor : public std::function<T&(unsigned int,decltype(RestD)...)>
{
public:
  Tensor(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<T&(unsigned int,decltype(RestD)...)>(a_input) { }
  void operator=(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) {((std::function<T&(unsigned int,decltype(RestD)...)>&)(*this)) = a_input;}
};

template<class T, unsigned int C, unsigned char... RestD>
class CTensor : public std::function<const T&(unsigned int,decltype(RestD)...)>
{
public:
  CTensor(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<const T&(unsigned int,decltype(RestD)...)>(a_input) { }
  void operator=(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input) {((std::function<const T&(unsigned int,decltype(RestD)...)>&)(*this)) = a_input;}
};



template <class T=double, unsigned int C=1, unsigned char D=1, unsigned char E=1> class RectMDArray 
{
public:
  /// Default constructor.
  RectMDArray();
  /// Constructs an RectMDArray over the Box a_box;
  RectMDArray(const Box& a_box);
  /// Defines a default-constructed RectMDArray.  
  void define(const Box& a_box);
  /// Copy constructor.  This is a deep copy, *this and a_srcArray are not sharing data
  RectMDArray(const RectMDArray<T,C,D,E>& a_srcArray);

  ///  Move constructor.  Good for cases were we return RectMDArray by value, but don't want an actual deep copy
  RectMDArray(RectMDArray<T,C,D,E>&& a_srcArray);

  /// slice constructor for RectMDArray, used by the slice functions
  RectMDArray(std::shared_ptr<T> a_data, T* a_ptr, const Box& a_box);
  /// Destructor.
  ~RectMDArray(); 
  /// Setting a RectMDArray to a constant value.
  void setVal(const T& a_val) const;
  /// Assignment operator. This is a deep copy, *this and a_srcArray are not sharing data
  RectMDArray& operator=(const RectMDArray<T,C,D,E>& a_srcArray);
  /// Gets box over which array is defined.

  /// Box over which this RectMDArray is defined.  will include it's ghost cells if LevelData built it that way.
  Box getBox() const {return m_box;};
  /// Copy on Intersection.
  void copyTo(RectMDArray<T,C,D,E>& a_dest) const;
  ///
  /**
     Copy  with shift.  only copy from cells in validBoxSrc 
     validBoxSrc comes in unshifted.
     shift = destBox - srcBox (the lower left corner of the array holders)
  */
  void copyTo(RectMDArray<T,C,D,E> & a_dest,
              const  Box           & a_validBoxSrc, 
              const  Point         & a_shift) const;

  /// Copy with a shift
  void copyTo(RectMDArray<T,C,D,E>& a_dest, const Point& a_shift) const;
  /// Shift in place.
  void shift(const Point a_pt){m_box.shift(a_pt);};
  /// Indexing operator. only works for scalar RectMDArray objects
  inline T& operator[](const Point& a_iv);
  inline const T& operator[](const Point& a_iv) const;

  /// Indexing operator for vector-valued RectMDArray objects.  Have assertion failure if returned type is not scalar.
  inline T& operator()(const Point& a_iv, unsigned int a_comp);
  inline const T& operator()(const Point& a_iv, unsigned int a_comp) const;
  inline T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d);
  inline const T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const;
  inline T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e);
  inline const T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const;

  /// Index with integer.
  inline T& operator[](int a_index) const;

  /// Simple Algebra operations for objects of common type. Defined on intersection.
  RectMDArray<T,C,D,E>& plus(const RectMDArray<T,C,D,E>& a_rhs);
  RectMDArray<T,C,D,E>& operator*=(T scale);
  RectMDArray<T,C,D,E>& axby(T a, const RectMDArray<T,C,D,E>& x, T b, const RectMDArray<T,C,D,E>& y);
  RectMDArray<T,C,D,E>& plus(T scale, const RectMDArray<T,C,D,E>& a_rhs);
  RectMDArray<T,C,D,E>& operator*=(const RectMDArray<T,C,D,E>& a_rhs);

  void print();
  inline size_t dataSize() const;
  inline bool defined() const;
  
  // I have these since currying to overloaded functions is annoyingly pedantic in C++.
  inline T& get(const Point& a_iv, unsigned int a_comp=0);
  inline T& get2(const Point& a_iv, unsigned int a_comp, unsigned char a_d);
  inline T& get3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e);
  inline const T& getConst(const Point& a_iv, unsigned int a_comp=0) const;
  inline const T& getConst2(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const;
  inline const T& getConst3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const;


  // not for public use.  but can't make C++ template friends do what I want
  std::shared_ptr<T> m_sliceData()  {return m_data;} 
  T* m_slicePtr() {return m_rawPtr;}


private:

 
  std::shared_ptr<T> m_data;
  T*  m_rawPtr;
  Box m_box;

 
};

/// sliceing.    building a shallow copy of data referencing a larger data holder.
template<class T, unsigned int C0, unsigned int C1>
RectMDArray<T,C1> slice(RectMDArray<T,C0>& a_original, const Interval& a_interval);

template<class T, unsigned int C0, unsigned int C1>
const RectMDArray<T,C1> slice(const RectMDArray<T,C0>& a_original, const Interval& a_interval);

template<class T, unsigned int C, unsigned char D0, unsigned char D1>
RectMDArray<T,C,D1> slice(RectMDArray<T,C,D0>& a_original, const Interval& a_interval);

template<class T, unsigned int C, unsigned char D0, unsigned char D1>
const RectMDArray<T,C,D1> slice(const RectMDArray<T,C,D0>& a_original, const Interval& a_interval);

template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
RectMDArray<T,C,D,E1> slice(RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval);

template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
const RectMDArray<T,C,D,E1> slice(const RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval);



#include "RectMDArrayImplem.H"  
#endif
