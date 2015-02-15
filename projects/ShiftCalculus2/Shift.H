#ifndef _SHIFT_H_
#define _SHIFT_H_
#include "Point.H"
//#include<array>

using namespace std;
class Shift
{
public:
  inline Shift(){m_shift = getZeros();};
  inline Shift(Point a_pt){m_shift = a_pt;};
  inline Shift operator*(Shift a_s){return Shift(m_shift + a_s.m_shift);};
  
  inline Point getExp(){return m_shift;};
  inline int getExp(int dir){return m_shift[dir];};
  Point m_shift;
  inline void print(){m_shift.print();};

// DQ: Added assignment operator for Shift class.
// Shift operator= (const Shift & x);
};
inline Shift getUnitShift(){return Shift(getOnes());};
// inline array<Shift,DIM> getShiftVec()
// {
//   array<Shift,DIM> ret;
//   for (int dir = 0; dir < DIM; dir++)
//     {
//       ret[dir] = Shift(getUnitv(dir));
//     }
//   return ret;
// };
// inline Shift operator^(array<Shift,DIM> a_shiftvec,Point a_exp)
// {
//   Shift ret;
//   for (int dir = 0; dir < DIM; dir++)
//     {
//       ret = ret * Shift((a_shiftvec[dir].m_shift)*a_exp[dir]);
//     }
//   return ret; 
// }
#endif
