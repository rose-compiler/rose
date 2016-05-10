#ifndef _STENCIL_H_
#define _STENCIL_H_
#include "SPACE.H"
// #include <array>
// #include <tuple>
// #include <iostream>
// #include "CH_Timer.H"
#include "vector_lite.h"
#include "simple_Shift.H"
#include "simple_RectMDArray.H"

using namespace std;

/// An object which contains a set of offsets and coefficients associated with the application of a stencil. 
template <class T> class Stencil
{
public:
  /// Default Constructor
  Stencil();

  // DQ added constructor (not clear why these are required).
  Stencil(pair<Point,double> a_pair);
  Stencil(pair<Point,int> a_pair);
  Stencil(pair<Shift,int> a_pair);

  /// Construct a stencil from a Shift, Coefficient pair.
  Stencil(pair<Shift,T> a_pair,
          Point a_destRefratio=getOnes(),
          Shift a_destShift=getZeros(),
          Point a_srcRefratio=getOnes());

#if 1
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Defines "*" operator on Stencils.
  /**
    The product of two Stencils is defined as the composition of the two. Two stencils with shifts in the same direction will be combined into one shift which has a coefficient equal to the sum of the original shifts. 
  */
    Stencil<T> operator*(const Stencil<T> a_stencil) const;

  /// Multiplication in place by a scalar of type T.
    void operator*=(const T& a_coef);

  /// Calculates the sum of two Stencils of the same type T. 
  /**
    \todo Currently * and + do the same thing. Check to see how they are meant to work. 
  */
    Stencil<T> operator+(const Stencil<T> a_stencil) const;

 // DQ: Added operator+= to simplify the syntax.
    Stencil<T> operator+=(const Stencil<T> a_stencil) const;

  /// Prints the information in *this. Note that print() does this also in a slightly different format. 
  /**
    Output format is: 
    coefs and offsets: <index> , <coefficient> , <shift> ; ... 
    sourceRef, destRef, and destShift: <srcRef> , <destRef> , <destShift> 
  */
    void stencilDump() const; 

  /// Sets the value of m_destRefratio.
 // void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};
    void setDestRefratio(Point a_pt);

  /// Sets the value of m_srcRefratio.
 // void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 
  void setSrcRefratio(Point a_pt);

  /// Sets the value of m_destShift.
 // void setDestShift(Point a_pt){m_destShift = a_pt;};
  void setDestShift(Point a_pt);

  /// \todo Not Implemented. Determine what this function is supposed to do.
 // Stencil makeInterpStencil(RectMDArray<Stencil>){};
    Stencil makeInterpStencil(RectMDArray<Stencil>);
#endif

  /// Defines how a Stencil operates on a RectMDArray.
  /**
    This is the secret sauce of making a Stencil into an operator, using C++ forwarding. A real extended DSL would allow us to use right-binding for operator(), but standard C++ cannot use right-binding, or return-type disambiguation. Instead, this operator returns a tuple containing this, a_phi, and a_bx. The application of the stencil to a_phi is handled by apply() through the += and |= operators.
    Syntactic Example:
    @code
    // build the desired stencil S...
    // initialize input and output RectMDArrays IN and OUT and the domain Box B...
    OUT += S(IN,B) // S(IN) is added to OUT in B
    // OR ... 
    OUT |= S(IN,B) // S(IN) replaces the data in OUT in B
    @endcode
    See the documentation for operator+= and operator|= in the file Stencil.H for additional documentation.
  */
    std::tuple<const Stencil<T>*, const RectMDArray<T>*, const Box*>
    operator()(const RectMDArray<T>& a_phi, const Box& a_bx) const;

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

  /// Applies a Stencil to a_phi and stores it in a_lofphi.
    static void apply(const Stencil<T>& a_stencil,
      const RectMDArray<T>& a_phi,
      RectMDArray<T>& a_lofPhi,
      const Box& a_bx);


  /// Returns the coefficients of the Stencil.
  const vector<T>&  getCoefs() const;

  /// Returns the offsets of the Stencil.
  const vector<Point>& getOffsets() const;

  /// Alternative printing function to stencilDump(). 
  void print() const;
#endif

  private:
#if 0
  vector<T> m_coef;///< Coefficients of the Stencil
  vector<Point> m_offsets; ///< Offsets associated with the Stencil. Each offset corresponds to a coefficient in m_coef.
  Point m_srcRefratio; 
  Point m_destRefratio;
  Point m_destShift;
  Stencil<T>(vector<T > a_vecT, vector<Point > a_vecPt,
   Point a_destRefratio=getOnes(),
   Point a_destShift=getZeros(),
   Point a_srcRefratio=getOnes());
#endif
};

// #include "StencilImplem.H"

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

/// Defines the product of a coefficient of type T and a shift as a Stencil.
/**
  This is a shortcut for creating Stencils simply using the syntax:\n
  @code
  Stencil<double> S = 3.7*shift 
  @endcode
  Where shift is a valid shift object.
*/
template <class T> Stencil<T> operator*(T a_coef, Shift a_shift);
#endif

/// Replacement operator used for Stencil application to a RectMDArray.
/**
  This operator is used for replacing the data of a_lofPhi within a box B with the output of the Stencil operation. Data not within B is unchanged.
*/
template <class T> RectMDArray<T>& operator|=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token);

/// Increment operator used for Stencil application to a RectMDArray.
/**
  This operator is used for summing the data of a_lofPhi within a box B with the output of the Stencil operation. Data not within B is unchanged.  
*/
template <class T> RectMDArray<T>& operator+=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token);

#if 0
// DQ: Commented out unused code to make the AST smaller and easier to debug.

/// Stencil application function.
/**
  This function is called by the += and |= operators. See the documentation of these operators in the Stencil.H file for more information.
  \bug Timer code does not appear to be working and has been commented out.
*/
// template <class T> void Stencil<T>::apply(const Stencil<T>& a_stencil, const RectMDArray<T>& a_phi, RectMDArray<T>& a_lofPhi, const Box& a_bx);

/// Applies a Stencil to a given component of a source RectMDArray and stores it in a given component of another RectMDArray.
template <class T, int SRCCOMP, int DSTCOMP> 
void componentApply(const Stencil<T> & a_stencil, const RectMDArray<T, SRCCOMP>& a_phi, RectMDArray<T, DSTCOMP>& a_lofPhi, const Box & a_bx, const int & a_srccomp, const int & a_dstcomp);
#endif

#endif
