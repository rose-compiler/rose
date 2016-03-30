#ifndef _RECTMDARRAY_H_
#define _RECTMDARRAY_H_
#include "simple_Box.H"
// #include <memory>
// #include <cassert>
// #include <functional>

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

/// A linear interval with integer bounds.
class Interval 
{
public:
  unsigned int low; ///< Lower bound of Interval
  unsigned int high;///< Upper bound of Interval

  /// Construct an Interval given upper and lower bounds as unsigned integers. 
  Interval(const unsigned int & a_low, const unsigned int & a_high);
};

/// Tensor object used to interface with data pointwise in a RectMDArray.
/**
  Tensor is used mostly for implementing pointwise operators on RectMDArrays. See the documentation for forall for more information.
*/
template<class T, unsigned int C, unsigned char... RestD>
// class Tensor : public std::function<T&(unsigned int,decltype(RestD)...)>
class Tensor // : public std::function<T&(unsigned int,decltype(RestD)...)>
{
public:
  /// Constructor. A Tensor is built from a function a_input with return type T
  // Tensor(std::function<T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<T&(unsigned int,decltype(RestD)...)>(a_input);
  //  Tensor(std::function<T&(unsigned int,decltype(RestD)...)>& a_input);

  /// Assignment operator. This is necessary for inheritance from std::function to work properly. 
  // void operator=(std::function<T&(unsigned int,decltype(RestD)...)>& a_input);
};

/// Tensor object used to interface with data pointwise in a constant RectMDArray.
/**
  Version of Tensor used with const functions. 
*/
template<class T, unsigned int C, unsigned char... RestD>
// class CTensor : public std::function<const T&(unsigned int,decltype(RestD)...)>
class CTensor // : public std::function<const T&(unsigned int,decltype(RestD)...)>
{
public:
  /// Constructor.
  // CTensor(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input) : std::function<const T&(unsigned int,decltype(RestD)...)>(a_input);
  //  CTensor(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input);

  /// Assignment operator. This is necessary for inheritance from std::function to work properly. 
  // void operator=(std::function<const T&(unsigned int,decltype(RestD)...)>& a_input);
};
#endif

/// Multidimensional Rectangular Array of data.
/**
  Contains an array of data of type T on a domain defined by a Box. Each data point is associated with a Point in the Box. C, D, and E provide the dimensionality of the data:
  C,D,E = 1:      Data is scalar
  C = 3, D,E = 1: Data is vector valued with 3 components
  C,D = 3, E = 1: Data is 3x3 matrix valued

  \tparam T Type of data in array
  \tparam C Number of components in first data index. Defaults to 1. 
  \tparam D Number of components in second data index. Defaults to 1. 
  \tparam E Number of components in third data index. Defaults to 1.
*/
template <class T=double, unsigned int C=1, unsigned char D=1, unsigned char E=1>
  class RectMDArray {
  public:
#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Default constructor.
    RectMDArray();
#endif

  /// Constructs a RectMDArray over the Box a_box. Data is USUALLY initialized as zero.
    /**
      When using this constructor, it is recommended that the user initialize the data manually:
      @code
      Box B = Box(getZeros(),getOnes()*7);  //Domain
      RectMDArray<double> R = RectMDArray<double>(B); //Construct array
      R.setVal(0);  //Initialize array values as 0. 
      @endcode
    */
    RectMDArray(const Box& a_box);

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Defines a default-constructed RectMDArray.
    /**
      Called by Constructor RectMDArray(const Box& a_box)
    */
    void define(const Box& a_box);

  /// Copy constructor.  This is a deep copy, *this and a_srcArray are not sharing data
    RectMDArray(const RectMDArray<T,C,D,E>& a_srcArray);

  /// Move constructor.  Good for cases were we return RectMDArray by value, but don't want an  actual deep copy
    RectMDArray(RectMDArray<T,C,D,E>&& a_srcArray);

  /// Slice constructor for RectMDArray, used by the slice functions
  /**
    Produces a RectMDArray whose data points to a slice of another RectMDArray's data. The data itself is not copied. 
    \param a_data Pointer to the first index of the full array of data.
    \param a_ptr Pointer to the first index of the data slice.
    \param a_box Domain of the data slice. 
  */
  //    RectMDArray(std::shared_ptr<T> a_data,T* a_ptr,const Box& a_box);

  /// Destructor.
    ~RectMDArray(); 

  /// Sets all values in a RectMDArray to a constant value.
  /**
    \param a_val A constant value. 
  */
    void setVal(const T& a_val) const;

  /// Assignment operator.
  /**
    This is USUALLY a deep copy; *this and a_srcArray are not sharing data. The exception is when a slice is being stored in a RectMDArray that has already been constructed.
  */
    RectMDArray& operator=(const RectMDArray<T,C,D,E>& a_srcArray);
#endif

  /// Gets box over which array is defined. 
  /**
    Includes it's ghost cells if LevelData built it that way.
  */
  Box getBox() const;

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Copy on Intersection.
  /**
    Copy the part of *this's data which intersects with the domain of a_dest's domain. 
  */
    void copyTo(RectMDArray<T,C,D,E>& a_dest) const;

  /// Copy on Intersection with a shift.
  /**
    Copy  with shift.  only copy from cells in validBoxSrc 
    validBoxSrc comes in unshifted. Used internally by LevelData. Not recommended for public use.
    shift = destBox - srcBox (the lower left corner of the array holders)
    \param a_dest Destination RectMDArray
    \param a_validBoxSrc Source domain. Must be a subset of the domain of *this.
    \param a_shift The shift that will be applied to a_dest.m_box.
  */
    void copyTo(RectMDArray<T,C,D,E> & a_dest,
      const  Box           & a_validBoxSrc, 
      const  Point         & a_shift) const;

  /// Copy with a shift
  /**
    For each point p in the intersection of *this and a_dest, copy the data at p+shift in *this to a_dest. Used internally by LevelData. Not recommended for public use.
    \param a_dest Destination RectMDArray.
    \param a_shift A Point interpreted as a shift vector.
  */
    void copyTo(RectMDArray<T,C,D,E>& a_dest,
      const Point&          a_shift) const;

  /// Shift in place.
  /**
    Shift the domain of *this using a_pt
    \param a_pt A Point interpreted as a shift vector.
  */
  void shift(const Point a_pt); 

  /// Indexing operator. only works for scalar RectMDArray objects
  /**
    Returns the data stored in *this corresponding to Point a_iv. Assertion error if a_iv is not in the domain of *this. 
    \param a_iv A Point in the domain of *this
  */
    inline T& operator[](const Point& a_iv);

  /// Indexing operator for const RectMDArrays. only works for scalar RectMDArray objects.
  /**
    Returns the data stored in *this corresponding to Point a_iv. Assertion error if a_iv is not in the domain of *this. 
    \param a_iv A Point in the domain of *this
  */
    inline const T& operator[](const Point& a_iv) const;

  /// Indexing operator for vector-valued RectMDArray objects with a single index. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp Integer corresponding to the desired component of the data at a_iv.
  */
    inline T& operator()(const Point& a_iv, unsigned int a_comp);

  /// Indexing operator for constant vector-valued RectMDArray objects with a single index. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp Integer corresponding to the desired component of the data at a_iv.
  */
    inline const T& operator()(const Point& a_iv, unsigned int a_comp) const;

  /// Indexing operator for vector-valued RectMDArray objects with two indices. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp First index of the desired component of the data at a_iv.
    \param a_d Second index of the desired component of the data at a_iv
  */
    inline T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d);

  /// Indexing operator for constant vector-valued RectMDArray objects with two indices. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp First index of the desired component of the data at a_iv.
    \param a_d Second index of the desired component of the data at a_iv
  */
    inline const T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const;

  /// Indexing operator for vector-valued RectMDArray objects with three indices. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp First index of the desired component of the data at a_iv.
    \param a_d Second index of the desired component of the data at a_iv
    \param a_e Third index of the desired component of the data at a_iv
  */
    inline T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e);

  /// Indexing operator for constant vector-valued RectMDArray objects with three indices. Assertion failure if returned type is not scalar.
  /**
    \param a_iv A Point in the domain of *this
    \param a_comp First index of the desired component of the data at a_iv.
    \param a_d Second index of the desired component of the data at a_iv
    \param a_e Third index of the desired component of the data at a_iv
  */
    inline const T& operator()(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const;

  /// Index with integer. Assertion failure if a_index is out of bounds. 
  /**
    \param a_index Integer valued index between 0 and *this.m_box.sizeOf()-1.
  */
    inline T& operator[](int a_index) const;

  /// Addition in place of two RectMDArrays. Defined on the intersection.
    RectMDArray<T,C,D,E>& plus(const RectMDArray<T,C,D,E>& a_rhs);

  /// Subtraction in place of two RectMDArrays. Defined on the intersection.
    RectMDArray<T,C,D,E>& minus(const RectMDArray<T,C,D,E>& a_rhs);

  /// Multiplication in place of two RectMDArrays. Defined on the intersection.
    RectMDArray<T,C,D,E>& times(const RectMDArray<T,C,D,E>& a_rhs);

  /// Division in place of two RectMDArrays. Defined on the intersection.
  /**
    Assertion error if division by 0 would occur.
  */
    RectMDArray<T,C,D,E>& divide(const RectMDArray<T,C,D,E>& a_rhs);

  /// Same as RectMDArray::plus(). Defines the "+=" operator on RectMDArrays.  Defined on the intersection.
    RectMDArray<T,C,D,E>& operator+=(const RectMDArray<T,C,D,E>& a_rhs);
#endif

  /// Same as RectMDArray::minus(). Defines the "-=" operator on RectMDArrays.  Defined on the intersection.
    RectMDArray<T,C,D,E>& operator-=(const RectMDArray<T,C,D,E>& a_rhs);

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Same as RectMDArray::times(). Defines the "*=" operator on RectMDArrays.  Defined on the intersection.
    RectMDArray<T,C,D,E>& operator*=(const RectMDArray<T,C,D,E>& a_rhs);

  /// Same as RectMDArray::divide(). Defines the "/=" operator on RectMDArrays.  Defined on the intersection.
  /**
    Assertion error if division by 0 is attempted.
  */
   RectMDArray<T,C,D,E>& operator/=(const RectMDArray<T,C,D,E>& a_rhs);

  ///Componentwise addition in place by scale.  
    RectMDArray<T,C,D,E>& operator+=(T scale);

  ///Componentwise subtraction in place by scale. 
    RectMDArray<T,C,D,E>& operator-=(T scale);

  ///Componentwise multiplication in place by scale. 
    RectMDArray<T,C,D,E>& operator*=(T scale);

  ///Componentwise division in place by scale. 
  /**
    Assertion error if scale = 0.
  */
    RectMDArray<T,C,D,E>& operator/=(T scale);

  // TEMPORARILY REMOVED

  // /// Adds the quantity a*x + b*y in place. Defined on the intersection.
  // /**
  //   param a Scale multiplying x
  //   param x A RectMDArray
  //   param b Scale multiplying y
  //   aram y A RectMDArray
  // */
  //   RectMDArray<T,C,D,E>& axby(T a, const RectMDArray<T,C,D,E>& x, T b, const RectMDArray<T,C,D,E>& y);

  // /// Adds a scaled RectMDArray to *this. Defined on the intersection.
  // /**
  //   param scale A scale factor.
  //   param a_rhs A RectMDArray
  // */
  //   RectMDArray<T,C,D,E>& plus(T scale, const RectMDArray<T,C,D,E>& a_rhs);

  /// Prints the domain box and the data in m_data
    void print();

  /// Prints the domain box and the data in m_data for constant RectMDArray
    void print() const;

  /// Returns the number of values stored in *this.
  /**
    Output is equal to m_box.sizeOf()*C*D*E
  */
    inline size_t dataSize() const;

  /// Returns true if *this has been defined.
    inline bool defined() const;
    
  // I have these since currying to overloaded functions is annoyingly pedantic in C++.

  /// Return the data associated with the Point a_iv. For use with scalar or vector valued data.
  /**
    \param a_iv A Point in the domain of *this.
    \param a_comp Desired component of the data. Defaults to 0 (for scalar data)
  */
    inline T& get(const Point& a_iv, unsigned int a_comp=0);

  /// Return the data associated with the Point a_iv. For use with data with two indices.
  /**
    \param a_iv A Point in the domain of *this.
    \param a_comp First index of the desired component of the data
    \param a_comp Second index of the desired component of the data
  */
    inline T& get2(const Point& a_iv, unsigned int a_comp, unsigned char a_d);

  /// Return the data associated with the Point a_iv. For use with data with three indices.
  /**
    \param a_iv A Point in the domain of *this.
    \param a_comp First index of the desired component of the data
    \param a_comp Second index of the desired component of the data
    \param a_comp Third index of the desired component of the data
  */
    inline T& get3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e);

  /// Same as RectMDArray::get() for constant RectMDArrays
    inline const T& getConst(const Point& a_iv, unsigned int a_comp=0) const;

  /// Same as RectMDArray::get2() for constant RectMDArrays
    inline const T& getConst2(const Point& a_iv, unsigned int a_comp, unsigned char a_d) const;

  /// Same as RectMDArray::get3() for constant RectMDArrays
    inline const T& getConst3(const Point& a_iv, unsigned int a_comp, unsigned char a_d, unsigned char a_e) const;


  // not for public use.  but can't make C++ template friends do what I want
  /// Not for Public use. DO NOT USE. 
  // std::shared_ptr<T> m_sliceData();

  /// Not for Public use. DO NOT USE. 
  T* m_slicePtr();
  /// Check if *this is a slice of another array.
  bool isSlice() const;
#endif

  private:
#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  std::shared_ptr<T> m_data; ///< Data array
  T*  m_rawPtr; ///< Raw pointer to the data
  Box m_box; ///< Box defining the domain of *this

  // Added this as a workaround for a bug involving assignment and slicing. 
  bool m_isSlice; ///< Flag which is true if *this is a slice of another RectMDArray (and is thus sharing data).
#endif
};


#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

/// Slice into a RectMDArray with vector valued (single index) data. Output is an alias to a subset of the input (shallow copy).
/**
  Note that only the lower bound of the Interval input is used. The upper bound of a_interval can be any value at all. \n
  To obtain generate an alias to the first component of a vector-valued array, the lower bound of a_interval must be 0 and the output array must have 1 component (see Example 1).\n
  To get the second component, the lower bound of a_interval must be 1 and the output array must have 1 component (see Example 2).\n
  To obtain the first and second components of a 3 component array, the lower bound of a_interval must be 0 and the output data must have 2 components (see Example 3).\n 

  Example Usage:
  @Code
  Point p1 = getZeros();
  Point p2 = getOnes();
  Box b = Box(p1,p2);
  RectMDArray<int,3> source = RectMDArray<int,3>(b);

  //Example 1:
  //Create alias to  first component of source:
  Interval I1 = Interval(0,1);
  RectMDArray<int,1> slice1<int,3,1>(source,I1)

  //Example 2:
  //Create alias to the second component of source:
  Interval I2 = Interval(1,2);
  RectMDArray<int,1> slice1<int,3,1>(source,I2)

  //Example 3:
  //Create alias to the second and third components of source:
  Interval I23 = Interval(1,3);
  RectMDArray<int,2> slice1<int,3,2>(source,I23)
  @endcode

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C0 Number of components in the input RectMDArray
  \tparam C1 Number of components in the output RectMDArray
  \param a_original The source RectMDArray with data of type T and C0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
  
*/
template<class T, unsigned int C0, unsigned int C1>
  RectMDArray<T,C1> slice(RectMDArray<T,C0>& a_original, const Interval& a_interval);

/// Slice into a constant RectMDArray with vector valued (single index) data. Output is an alias to a subset of the input (shallow copy).
/**
  See documentation for the non-constant version of slice for notes and example usage. 

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C0 Number of components in the input RectMDArray
  \tparam C1 Number of components in the output RectMDArray
  \param a_original The source constant RectMDArray with data of type T and C0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
*/
template<class T, unsigned int C0, unsigned int C1>
  const RectMDArray<T,C1> slice(const RectMDArray<T,C0>& a_original, const Interval& a_interval);

/// Slice into a RectMDArray with matrix valued (doubly indexed) data. Output is an alias to a subset of the input (shallow copy).
/**
  This version of slice produces alias to a C x D1 dimensional subset of the C x D0 dimensional input RectMDArray. See documentation for the vector valued version of slice() for additional example usage and notes. 

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C Number of the first set of components of both the input and output RectMDArrays
  \tparam D0 Number of the second set of components of the input RectMDArray
  \tparam D1 Number of the second set of components of the output RectMDArray
  \param a_original The source constant RectMDArray with data of type T and C x D0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
*/
template<class T, unsigned int C, unsigned char D0, unsigned char D1>
  RectMDArray<T,C,D1> slice(RectMDArray<T,C,D0>& a_original, const Interval& a_interval);

/// Slice into a constant RectMDArray with matrix valued (doubly indexed) data. Output is an alias to a subset of the input (shallow copy).
/**
  See the non-constant input version for notes on usage. See documentation for the vector valued version of slice() for additional example usage and notes. 

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C Number of the first set of components of both the input and output RectMDArrays
  \tparam D0 Number of the second set of components of the input RectMDArray
  \tparam D1 Number of the second set of components of the output RectMDArray
  \param a_original The source constant RectMDArray with data of type T and C x D0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
*/
template<class T, unsigned int C, unsigned char D0, unsigned char D1>
  const RectMDArray<T,C,D1> slice(const RectMDArray<T,C,D0>& a_original, const Interval& a_interval);

/// Slice into a RectMDArray with three sets of components (three indices). Output is an alias to a subset of the input (shallow copy).
/**
  This version of slice produces alias to a C x D x E1 dimensional subset of the C x D x E0 dimensional input RectMDArray. See documentation for the vector valued version of slice() for additional example usage and notes. 

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C Number of the first set of components of both the input and output RectMDArrays
  \tparam D Number of the second set of components of both the input and output RectMDArrays
  \tparam E0 Number of the third set of components of the input RectMDArray
  \tparam E1 Number of the third set of components of the output RectMDArray
  \param a_original The source constant RectMDArray with data of type T and C x D x E0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
*/
template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
  RectMDArray<T,C,D,E1> slice(RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval);

/// Slice into a constant RectMDArray with three sets of components (three indices). Output is an alias to a subset of the input (shallow copy).
/**
  See the non-constant input version for notes on usage. See documentation for the vector valued version of slice() for additional example usage and notes. 

  \tparam T A Class. Type of data stored by input and output RectMDArrays
  \tparam C Number of the first set of components of both the input and output RectMDArrays
  \tparam D Number of the second set of components of both the input and output RectMDArrays
  \tparam E0 Number of the third set of components of the input RectMDArray
  \tparam E1 Number of the third set of components of the output RectMDArray
  \param a_original The source constant RectMDArray with data of type T and C x D x E0 components
  \param a_interval An Interval object. The lower bound of a_interval determines which components of a_original will be be sliced into the output. 
*/
template<class T, unsigned int C, unsigned char D, unsigned char E0, unsigned char E1>
  const RectMDArray<T,C,D,E1> slice(const RectMDArray<T,C,D,E0>& a_original, const Interval& a_interval);

/// Pointwise operator. Applies F to the single indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest.
/** 
  \tparam T Data type of the RectMDArray arguments
  \tparam Cdest Number of components in the destination RectMDArray
  \tparam Csrc Number of components in the source RectMDArray
  \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
  \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
  \param a_src Source RectMDArray. This is the array that F acts on.
  \param F A pointwise function with a Tensor and a CTensor as arguments which acts on a_src.   F can be any externally defined function that bears the following signature:
  @code
  void F(Tensor<T,Cdest>& a_OUT, CTensor<T, Csrc>& a_IN);
  @endcode
  \param  a_box A Box which defines the domain in point space of the operation. 

  When writing F, the Tensor object may be considered to represent all of the data in a RectMDArray at a given point. This version of forall() assumes that both RectMDArrays are singly indexed (or scalar) arrays. Below is a simple example of a valid function input to forall(). This function permutes the components of a 3 component vector datatype: 
  
  @code
  void PermuteF(Tensor<double,3>& a_OUT, CTensor<double, 3>& a_IN) {
    //a_IN(i) is the ith component of the vector data at some point, and similarly with a_OUT(i).
    a_OUT(0) = a_IN(1);
    a_OUT(1) = a_IN(2);
    a_OUT(2) = a_IN(0);
  }
  @endcode
*/
template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
void forall(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box);

//REDUNDANT
//******************************************************************************************************************
// /// Pointwise operator. Applies F to the single indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest. Also outputs the maximum value of the output of F. This implementation uses arrays rather than Tensors.
// /**
//   In this case, F has a return value. The maximum of this return value over a_box will be returned by this function.

//   \tparam T Data type of the RectMDArray arguments
//   \tparam Cdest Number of components in the destination RectMDArray
//   \tparam Csrc Number of components in the source RectMDArray
//   \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
//   \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
//   \param a_src Source RectMDArray. This is the array that F acts on.
//   \param F A pointwise function with two T-type arrays of size Cdest and Csrc respectively. F can be any externally defined function that bears the following signature:
//   @code
//   void F(T destv[Cdest], T srcv[Csrc]);
//   @endcode
//   \param  a_box A Box which defines the domain in point space of the operation. 
// */
// template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
// T forall_vect_max(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box);

// /// Pointwise operator. Applies F to the single indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest. 
// /**
//   Identical to the forall() function of the same signature except that this version is not implemented with Tensors.

//   \tparam T Data type of the RectMDArray arguments
//   \tparam Cdest Number of components in the destination RectMDArray
//   \tparam Csrc Number of components in the source RectMDArray
//   \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
//   \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
//   \param a_src Source RectMDArray. This is the array that F acts on.
//   \param F A pointwise function with two T-type arrays of size Cdest and Csrc respectively. F can be any externally defined function that bears the following signature:
//   @code
//   void F(T destv[Cdest], T srcv[Csrc]);
//   @endcode
//   \param  a_box A Box which defines the domain in point space of the operation.
// */
// template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
// T forall_vect(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box);
//*******************************************************************************************************************

/// Pointwise operator. Applies F to the triple indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest.
/**
  Alteration of forall() that operates on a triply indexed RectMDArray. Implemented using Tensors.

  \tparam T Data type of the RectMDArray arguments
  \tparam C Number of components in the first index of BOTH the input and output RectMDArrays.
  \tparam D Number of components in the second index of the input RectMDArray.
  \tparam E Number of components in the third index of the input RectMDArray.
  \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
  \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
  \param a_src Source RectMDArray. This is the array that F acts on.
  \param F A pointwise function with a Tensor and a CTensor as arguments which acts on a_src.   F can be any externally defined function that bears the following signature:
  @code
  void F(Tensor<T,C>& a_OUT, CTensor<T,C,D,E>& a_IN);
  @endcode
  \param  a_box A Box which defines the domain in point space of the operation. 
*/
template<class T, unsigned int C, unsigned char D, unsigned char E, typename Func> 
void forall(RectMDArray<T,C>& a_dest, const RectMDArray<T,C,D,E>& a_src, const Func& F, const Box& a_box);

/// Pointwise operator. Applies F to the singly indexed RectMDArray a_src in the domain a_box and stores the result in the doubly indexed RectMDArray a_dest.
/**
  Alteration of forall() that operates on a singly indexed RectMDArray and produces a doubly indexed RectMDArray. Implemented using Tensors.\n
  An example of an F that would do this is the calculation of the velocity gradient (a second order tensor) from the velocity (a first order tensor). 

  \tparam T Data type of the RectMDArray arguments
  \tparam Cdest Number of components in the first index of the output RectMDArray
  \tparam Csrc Number of components in the first index of the input RectMDArray
  \tparam D Number of components in the second index of the output RectMDArray.
  \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
  \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
  \param a_src Source RectMDArray. This is the array that F acts on.
  \param F A pointwise function with a Tensor and a CTensor as arguments which acts on a_src.   F can be any externally defined function that bears the following signature:
  @code
  void F(Tensor<T,C>& a_OUT, CTensor<T,C,D,E>& a_IN);
  @endcode
  \param  a_box A Box which defines the domain in point space of the operation. 
*/
template<class T, unsigned int Cdest, unsigned int Csrc, unsigned char D, typename Func> 
void forall(RectMDArray<T,Cdest,D>& a_dest, const RectMDArray<T,Csrc>& a_src, const Func& F, const Box& a_box);

/// Pointwise operator. Applies F to the doubly indexed RectMDArray a_src in the domain a_box and stores the result in the scalar RectMDArray a_dest.
/**
  Alteration of forall() that operates on a doubly indexed RectMDArray and produces a scalar RectMDArray. Implemented using Tensors.\n
  An example of an F that would do this is the calculation of the pointwise trace of a 2D matrix valued array.

  \tparam T Data type of the RectMDArray arguments
  \tparam C Number of components in the first index of the input RectMDArray
  \tparam D Number of components in the second index of the input RectMDArray.
  \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
  \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
  \param a_src Source RectMDArray. This is the array that F acts on.
  \param F A pointwise function with a scalar T and a CTensor as arguments which acts on a_src. F can be any externally defined function that bears the following signature:
  @code
  void F(T& a_OUT, CTensor<T,C,D>& a_IN);
  @endcode
  \param  a_box A Box which defines the domain in point space of the operation. 
*/
template<class T, unsigned int C, unsigned char D, typename Func> 
void forall(RectMDArray<T>& a_dest, const RectMDArray<T,C,D>& a_src, const Func& F, const Box& a_box);

/// In place pointwise operator. Applies F to the (scalar) RectMDArray a_dest for every point in a_box.
/**
  Alteration of forall() that applies F in place on a scalar RectMDArray. No Tensors are necessary for this version.

  \tparam T Data type of the RectMDArray arguments
  \tparam Func Function datatype. Usually this parameter will be filled with decltype(F).
  \param a_dest Destination RectMDArray. This is where the result of F(a_src) is stored.
  \param F A pointwise function with a Tensor and a CTensor as arguments which acts on a_src.   F can be any externally defined function that bears the following signature:
  @code
  void F(T& a_IN);
  @endcode
  \param  a_box A Box which defines the domain in point space of the operation. 
*/
template<class T, typename Func> 
void forall(RectMDArray<T>& a_dest, const Func& F, const Box& a_box);

// REDUNDANT
//***************************************************************************************************************
// /// Pointwise operator. Applies F to the singly indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest. 
// template<class T, unsigned int C, typename Func> 
// void forall(RectMDArray<T,C>& a_dest, const RectMDArray<T,C>& a_src, const Func& F, const Box& a_box);
//***************************************************************************************************************

// REDUNDANT
//***************************************************************************************************************
// /// Pointwise operator. Applies F to the singly indexed COARSE RectMDArray a_srcCoar in the domain a_boxCoar and stores the result in the singly indexed REFINED RectMDArray a_destFine. 
// template<class T, unsigned int C, typename Func> 
// void forall_CToF(RectMDArray<T,C>& a_destFine, const RectMDArray<T,C>& a_srcCoar, 
//   const Func& F, const Box& a_boxCoar, const int& a_refRat);
//***************************************************************************************************************

// REDUNDANT
//***************************************************************************************************************
// /// Pointwise operator. Applies F to the singly indexed RectMDArray a_src in the domain a_box and stores the result in the singly indexed RectMDArray a_dest. Implemented with striding functionality.
// template<class T, unsigned int C, typename Func> 
// void forall_stride(RectMDArray<T,C>& a_dest, const RectMDArray<T,C>& a_src, const Func& F, const Box& a_box,
//   const int& a_stride, const int & a_start);
//***************************************************************************************************************

// REDUNDANT
//***************************************************************************************************************
// template<class T, typename Func> 
// T forall_max_scal(RectMDArray<T>& a_dest, const RectMDArray<T>& a_src, const Func& F, const Box& a_box);

// template<class T, typename Func> 
// void forall_scal(RectMDArray<T>& a_dest, const RectMDArray<T>& a_src, const Func& F, const Box& a_box);

// template<class T, unsigned int Cdest, unsigned int Csrc, typename Func> 
// T forall_max(RectMDArray<T,Cdest>& a_dest, const RectMDArray<T,Csrc>& a_src,const Func& F, const Box& a_box);
//***************************************************************************************************************

/// Returns the value of a_src in a_box with the largest absolute value.
/**
  \tparam T Data type of a_src
  \param a_src Input data
  \param a_box Domain 
*/
template<class T> T& abs_max(RectMDArray<T>& a_src, const Box& a_box);

/// Returns a tuple containing the value of a_src in a_box with the largest absolute value and the Point at which this value is attained.
/**
  \tparam T Data type of a_src
  \param a_src Input data
  \param a_box Domain 
*/
template<class T> tuple<T&, Point> abs_argmax(const RectMDArray<T>& a_src, const Box& a_box);
#endif


// #include "RectMDArrayImplem.H"  
#endif
