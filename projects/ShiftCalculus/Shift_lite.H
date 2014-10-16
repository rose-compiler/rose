#ifndef _SHIFT_H_
#define _SHIFT_H_

#include "Point_lite.H"
//#include<array>

using namespace std;
class Shift
{
public:
  Shift();
  Shift(Point a_pt);
  Shift operator*(Shift a_s);
  
  Point getExp();
  int getExp(int dir);
  Point m_shift;
  void print();
};

Shift getUnitShift();

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
