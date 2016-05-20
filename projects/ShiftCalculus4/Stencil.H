#ifndef _STENCIL_H_
#define _STENCIL_H_
#include "SPACE.H"
#include <array>
#include <tuple>
#include <iostream>
#include "Shift.H"
#include "RectMDArray.H"
using namespace std;


template <class T> class Stencil
{
public:
  Stencil();
  Stencil(pair<Shift,T> a_pair,
          Point a_destRefratio=getOnes(),
          Shift a_destShift=getZeros(),
          Point a_srcRefratio=getOnes());
  Stencil<T> operator*(const Stencil<T> a_stencil) const;
  void operator*=(const T& a_coef);
  Stencil<T> operator+(const Stencil<T> a_stencil) const;
       // DQ (2/15/2015): Added operator+=() to support clearer updates of an existing object for compile-time analysis.
       // Stencil<T> operator+= (const Stencil<T> a_stencil) const;
  Stencil<T> operator+= (const Stencil<T> a_stencil);

  void stencilDump() const; 
  void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};
  void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 
  void setDestShift(Point a_pt){m_destShift = a_pt;};
  Stencil makeInterpStencil(RectMDArray<Stencil>){};

  /// This is the secrect sauce of making a Stencil into an operator, using C++ forwarding.
  // A real extended DSL would allow us to use right-binding for operator(), but standard C++ cannot
  // use right-binding, or return-type disambiguation.
  std::tuple<const Stencil<T>*, const RectMDArray<T>*, const Box*>
    operator()(const RectMDArray<T>& a_phi,
	       const Box& a_bx) const 
  {
    auto t = std::make_tuple(this, &a_phi, &a_bx);
    return t;
  }
  static void apply(const Stencil<T>& a_stencil,
		    const RectMDArray<T>& a_phi,
		    RectMDArray<T>& a_lofPhi,
		    const Box& a_bx);
  static void apply2(const Stencil<T>& a_stencil,
		    const RectMDArray<T>& a_phi,
		    RectMDArray<T>& a_lofPhi,
		    const Box& a_bx);

// DQ (2/22/2015): The DSL compiler needs to be able to access the stencil data members (make these public for now).
// private:
public:
  vector<T> m_coef;
  vector<Point> m_offsets;
  Point m_srcRefratio;
  Point m_destRefratio;
  Point m_destShift;
  Stencil<T>(vector<T > a_vecT, vector<Point > a_vecPt,
                 Point a_destRefratio=getOnes(),
                 Point a_destShift=getZeros(),
                 Point a_srcRefratio=getOnes());


};
  
#include "StencilImplem.H"

template <class T> Stencil<T> 
operator*(T a_coef, Shift a_shift)
{return Stencil<T>(pair<Shift,T>(a_shift,a_coef));};

template <class T>
RectMDArray<T>& operator|=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token)
{
  const Box& b=*(std::get<2>(a_token));
  forall<T>(a_lofPhi,[](T& t){ t=0; }, b);
  Stencil<T>::apply(*(std::get<0>(a_token)), *(std::get<1>(a_token)), a_lofPhi, b);
  return a_lofPhi;
}

template <class T>
RectMDArray<T>& operator+=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token)
{
  const Box& b=*(std::get<2>(a_token));
  assert(a_lofPhi.getBox().contains(b));
  Stencil<T>::apply(*(std::get<0>(a_token)), *(std::get<1>(a_token)), a_lofPhi, b);
  return a_lofPhi;
}



template <class T>
void Stencil<T>::apply(const Stencil<T>& a_stencil,
		       const RectMDArray<T>& a_phi,
		       RectMDArray<T>& a_lofPhi,
		       const Box& a_bx)
{
    CH_TIMERS("applyOpStencil");
    CH_TIMER("innerloop",t1);
    const vector<T> & coef = a_stencil.m_coef;
    const vector<Point> & offsets = a_stencil.m_offsets;
    const Point& srcRefratio = a_stencil.m_srcRefratio;
    const Point& destRefratio = a_stencil.m_destRefratio;
    const Point& destShift = a_stencil.m_destShift;

    int ncoef = coef.size();
    int tuple[DIM];
    int n = a_bx.getHighCorner()[0] - a_bx.getLowCorner()[0]+1;
    int incp[DIM];
    incp[0] = 1;
    Box bxl = a_lofPhi.getBox();
    Box bxp = a_phi.getBox();
    for (int l=1;l<DIM;l++)
      {
        incp[l] = incp[l-1]*
          (bxp.getHighCorner()[l-1] - bxp.getLowCorner()[l-1]+1);
        tuple[l] = a_bx.getHighCorner()[l];
      }
    tuple[0]=a_bx.getLowCorner()[0];
    Point hicross(tuple);
    Box cross(a_bx.getLowCorner(),
              a_bx.getHighCorner()*(getOnes()-getUnitv(0))
              +getUnitv(0)*a_bx.getLowCorner()[0]);
    vector<int> srcOffset;
    vector<vector<int > > srcOffsetVec;
 
    srcOffset.resize(coef.size());
    srcOffsetVec.resize(coef.size());
    int size = cross.sizeOf();
    int kbasel0 = 0;
    CH_START(t1);
    int npts = 0;
    int nstencil = 0;
    for (int isten = 0; isten < coef.size();isten++)
      {
        srcOffset[isten] = 0;
        for (int dir = 0; dir < DIM ; dir++)
          {
            srcOffset[isten] += offsets[isten][dir]*incp[dir];
          }
      }
    int desdisp = destRefratio[0];
    int srcdisp = srcRefratio[0];
    int destshift = destShift[0];
    for (int kperp = 0; kperp < size; kperp++)
      {
        Point pt = cross.getPoint(kperp);
        int kbasel = bxl.getIndex(pt*destRefratio + destShift); 
        int kbasep = bxp.getIndex(pt*srcRefratio);
          
        for (int isten = 0;isten < srcOffset.size(); isten++)
          {
            for (int k0 = 0;k0 < n;k0++)              
              {
                int offset =  srcOffset[isten];   
                int k0dest = k0*desdisp;

                int k0src = k0*srcdisp;
                a_lofPhi[kbasel + k0dest] += 
                  coef[isten]*a_phi[kbasep + k0src + offset];
              }
          }
      }
     CH_STOP(t1);
}


#endif
