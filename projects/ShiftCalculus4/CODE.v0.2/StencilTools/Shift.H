#ifndef _SHIFT_H_
#define _SHIFT_H_
#include "SPACE.H"
#include "Point.H"
#include <array>

using namespace std;
/// A direction in space represented by a Point object.
class Shift
{
public:
  /// Default Constructor
  inline Shift(){m_shift = getZeros();};
  /// Construct a Shift using a Point
  inline Shift(Point a_pt){m_shift = a_pt;};
  /// Defines the "*" operator between two Shift objects.
  /**
    The product of two Shifts results in a new Shift which is the sum of the two base objects' vectors. 
  */
  inline Shift operator*(Shift a_s){return Shift(m_shift + a_s.m_shift);};
  
  /// Retrieves the data in *this.
  inline Point getExp(){return m_shift;};

  /// Retrieves the data corresponding to direction dir in *this. 
  inline int getExp(int dir){return m_shift[dir];};

  Point m_shift; ///< Direction of the Shift.

  /// Print method. 
  /**
    Calls Point.print()
  */
  inline void print(){m_shift.print();};
};

////  Implementation
/// Creates a Shift with m_shift = (1,1,...,1)
inline Shift getUnitShift(){return Shift(getOnes());};

/// Creates an array of unit shift objects.
/**
  The k'th component of the output array is a Shift object with m_shift(j) = \f$\delta_j^k\f$.
*/
inline array<Shift,DIM> getShiftVec()
{
  array<Shift,DIM> ret;
  for (int dir = 0; dir < DIM; dir++)
    {
      ret[dir] = Shift(getUnitv(dir));
    }
  return ret;
};

/// Produce a Shift equal to the sum of the elements of a_shiftvec, each weighted by the components of a_exp.
inline Shift operator^(array<Shift,DIM> a_shiftvec,Point a_exp)
{
  Shift ret;
  for (int dir = 0; dir < DIM; dir++)
    {
      ret = ret * Shift((a_shiftvec[dir].m_shift)*a_exp[dir]);
    }
  return ret; 
}
#endif
