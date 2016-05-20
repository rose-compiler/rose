

#define assert(x)

#include <vector>
#include <memory>

// DQ (2/3/2015): This is not required for g++ using c++11 mode, but is required for ROSE.
#include <array>

#include "Box.H"
#include "SPACE.H"
#include "Point.H"
#include "Shift.H"
#include "RectMDArray.H"
using namespace std;

template <class T> class Stencil
   {
     public:
          Stencil();
          Stencil(pair<Shift,T> a_pair,Point a_destRefratio=getOnes(),Shift a_destShift=getZeros(),Point a_srcRefratio=getOnes());
          Stencil<T> operator*(const Stencil<T> a_stencil) const;
          void operator*=(const T& a_coef);
          Stencil<T> operator+(const Stencil<T> a_stencil) const;
          void stencilDump() const; 
          void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};
          void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 
          void setDestShift(Point a_pt){m_destShift = a_pt;};
          Stencil makeInterpStencil(RectMDArray<Stencil>){ return *this; };

       // This is the secrect sauce of making a Stencil into an operator, using C++ forwarding.
       // A real extended DSL would allow us to use right-binding for operator(), but standard C++ cannot
       // use right-binding, or return-type disambiguation.
          std::tuple<const Stencil<T>&&, const RectMDArray<T>&&,const Box&&> operator()(const RectMDArray<T>& a_phi,const Box& a_bx) 
             {
               return std::make_tuple(*this, a_phi, a_bx);
             }

          static void apply(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx);
          static void apply2(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx);

       // DQ (2/15/2015): Added operator+=() to support clearer updates of an existing object for compile-time analysis.
       // Stencil<T> operator+= (const Stencil<T> a_stencil) const;
          Stencil<T> operator+= (const Stencil<T> a_stencil);

     private:
          vector<T> m_coef;
          vector<Point> m_offsets;
          Point m_srcRefratio;
          Point m_destRefratio;
          Point m_destShift;
          Stencil<T>(vector<T > a_vecT, vector<Point > a_vecPt,Point a_destRefratio=getOnes(),Point a_destShift=getZeros(),Point a_srcRefratio=getOnes());
          Box m_extent;
          mutable std::vector<unsigned int> m_indexer;
   };
  
template <class T> Stencil<T> operator*(T a_coef, Shift a_shift) { return Stencil<T>(pair<Shift,T>(a_shift,a_coef)); }

template <class T>
RectMDArray<T>& operator|=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>&&, const RectMDArray<T>&&,const Box&& >& a_token)
   {
     return a_lofPhi;
   }

#include "StencilImplem.H"

template <class T> void Stencil<T>::apply(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}

template<class T, unsigned char D> class Loop
   {
     Loop(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}
   };

template<class T> class Loop<T,0>
   {
     Loop(const Stencil<T>& a_stencil,const RectMDArray<T>& a_phi,RectMDArray<T>& a_lofPhi,const Box& a_bx) {}
   };

template <class T> void Stencil<T>::apply2(const Stencil<T>& a_stencil, const RectMDArray<T>& a_phi, RectMDArray<T>& a_lofPhi, const Box& a_bx) {}

template <class TSrc,class TDest,class F>
class PointwiseOperator
   {
     public:
          PointwiseOperator(){};
          RectMDArray<TDest> operator()(F a_f,Box& a_bx, int n, ...)
             {
               RectMDArray<TDest> fOfPhi(a_bx);
               return fOfPhi;
             }
   };

// function to initialize the src data
void initialize(RectMDArray<double>& patch);
