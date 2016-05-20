#ifndef _SHIFT_H_
#define _SHIFT_H_
#include "SPACE.H"
#include "simple_Point.H"
// #include <array>

using namespace std;
/// A direction in space represented by a Point object.
class Shift
{
public:
  /// Default Constructor
  // inline Shift(){m_shift = getZeros();};
  Shift();
  /// Construct a Shift using a Point
  // inline Shift(Point a_pt){m_shift = a_pt;};
  Shift(Point a_pt);
  /// Defines the "*" operator between two Shift objects.
  /**
    The product of two Shifts results in a new Shift which is the sum of the two base objects' vectors. 
  */
  // inline Shift operator*(Shift a_s){return Shift(m_shift + a_s.m_shift);};
  Shift operator*(Shift a_s);
  
  /// Retrieves the data in *this.
  // inline Point getExp(){return m_shift;};
  Point getExp();

  /// Retrieves the data corresponding to direction dir in *this. 
  // inline int getExp(int dir){return m_shift[dir];};
  int getExp(int dir);

  Point m_shift; ///< Direction of the Shift.

  /// Print method. 
  /**
    Calls Point.print()
  */
  // inline void print(){m_shift.print();};
  void print();
};

////  Implementation
/// Creates a Shift with m_shift = (1,1,...,1)
// inline Shift getUnitShift(){return Shift(getOnes());};
Shift getUnitShift();

/// Creates an array of unit shift objects.
/**
  The k'th component of the output array is a Shift object with m_shift(j) = \f$\delta_j^k\f$.
*/
array<Shift,DIM> getShiftVec();

/// Produce a Shift equal to the sum of the elements of a_shiftvec, each weighted by the components of a_exp.
Shift operator^(array<Shift,DIM> a_shiftvec,Point a_exp);
#endif
