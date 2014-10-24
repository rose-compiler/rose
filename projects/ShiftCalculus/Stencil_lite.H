#ifndef _STENCIL_H_
#define _STENCIL_H_

// #include <iostream>

#include "Shift_lite.H"
#include "RectMDArray_lite.H"


// using namespace std;

//The main stencil class. A stencil consists of a collection
// of shifts (integer offsets from the destination discrete point) 
// and a coefficient associated with each shift. The number of shifts
// in a stencila can be arbitrarily small or large. Stencil class knows
// all the shifts and coefficients, and it also knows ratios of
// the discretization distance between two consecutive points in the 
// source and the destination. The ratio is important only when AMR
// is in use.

// "+" and "*" are two operators defined on stencils
// for S=S1+S2, S has the union of all shifts in S1 and S2.
// if they have a common shift with c1 as its coefficient in S1
// and c2 as the coefficient in S2, then the shift appears only
// once in S, and the coefficient associated with it becomes c1+c2.
// for S=S1*S2, there is convolution between S1 and S2 where the
// vectors representing the shifts undergo pointwise addition 
// and the corresponding coefficients
// are multiplied

template <class TCoef> class Stencil
   {
     public:
          Stencil();

       // constructor with a single shift
          Stencil(pair<Shift,TCoef> a_pair,
               Point a_destRefratio = getOnes(),
               Shift a_destShift    = getZeros(),
               Point a_srcRefratio  = getOnes());

       // constructor with multiple shifts
          Stencil<TCoef>(vector<pair<Shift,TCoef> > a_shifts,
               Point a_destRefratio = getOnes(),
               Shift a_destShift    = getZeros(),
               Point a_srcRefratio  = getOnes());  

          Stencil<TCoef> operator*(const Stencil<TCoef> a_stencil) const;
          void operator*=(const TCoef& a_coef);
          Stencil<TCoef> operator+(const Stencil<TCoef> a_stencil) const;

#if 0
  void stencilDump() const; 
  void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};
  void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 
  void setDestShift(Point a_pt){m_destShift = a_pt;};

// DQ: Added return statement to avoid compiler warning.
  Stencil makeInterpStencil(RectMDArray<Stencil>){ return Stencil(); };

  vector<TCoef> m_coef;
  vector<Point> m_offsets;
  Point m_srcRefratio;
  Point m_destRefratio;
  Point m_destShift;
  Stencil<TCoef>(vector<TCoef > a_vecT, vector<Point > a_vecPt,
                 Point a_destRefratio=getOnes(),
                 Point a_destShift=getZeros(),
                 Point a_srcRefratio=getOnes());
#endif
   };
  
// #include "StencilImplem.H"

template <class TCoef> Stencil<TCoef> 
operator*(TCoef a_coef, Shift a_shift);

//The stencil operator class which applies the stencil to the
// source box to output destination box with the result
template <class TCoef, class TSrc, class TDest> 
class StencilOperator
   {
     public:
          StencilOperator(){};
          void operator()(const Stencil<TCoef>& a_stencil,
               const RectMDArray<TDest>& a_LOfPhi,
               const RectMDArray<TSrc>& a_phi,
               const Box& a_bx);
   };

// //a function that can be applied pointwise
// template <class TSrc,class TDest,class F>
// class PointwiseOperator
// {
//  public:
//   PointwiseOperator(){};
//   void operator()(
//              F a_f,
//              RectMDArray<TDest>& a_fOfPhi, 
//              Box& a_bx, int n, ...)
//   {
//     int i;
//     va_list vl;
//     va_start(vl,n);
//     vector<RectMDArray<TSrc>* > vecvals;
//     for (i=0;i<n;i++)
//       {
//         RectMDArray<TSrc>* val=va_arg(vl,RectMDArray<TSrc>*);
//         vecvals.push_back(val);
//       }
//     for (Point pt = a_bx.getLowCorner();a_bx.notDone(pt);a_bx.increment(pt))
//       {
//         vector<TDest> f_arg;
//         for (i = 0; i < n; i++)
//           {
//             f_arg.push_back((*(vecvals[i]))[pt]);
//           }
//         a_fOfPhi[pt] = a_f(f_arg);
//       }
//   };
// };
#endif
