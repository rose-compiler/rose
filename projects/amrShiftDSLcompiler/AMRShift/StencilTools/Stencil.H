#ifndef _STENCIL_H_
#define _STENCIL_H_
#include "SPACE.H"
#include <array>
#include <tuple>
#include <iostream>
#include "CH_Timer.H"
#include "Shift.H"
#include "RectMDArray.H"

using namespace std;

/// An object which contains a set of offsets and coefficients associated with the application of a stencil. 
#pragma DSL keyword
template <class T> class Stencil
{
public:
  /// Default Constructor
  Stencil();

  /// Construct a stencil from a Shift, Coefficient pair.
  Stencil(pair<Shift,T> a_pair,
          Point a_destRefratio=getOnes(),
          Shift a_destShift=getZeros(),
          Point a_srcRefratio=getOnes());
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

  /// Prints the information in *this. Note that print() does this also in a slightly different format. 
  /**
    Output format is: 
    coefs and offsets: <index> , <coefficient> , <shift> ; ... 
    sourceRef, destRef, and destShift: <srcRef> , <destRef> , <destShift> 
  */
    void stencilDump() const; 

  /// Sets the value of m_destRefratio.
    void setDestRefratio(Point a_pt){m_destRefratio = a_pt;};

  /// Sets the value of m_srcRefratio.
    void setSrcRefratio(Point a_pt){m_srcRefratio = a_pt;}; 

  /// Sets the value of m_destShift.
    void setDestShift(Point a_pt){m_destShift = a_pt;};

  /// \todo Not Implemented. Determine what this function is supposed to do.
    Stencil makeInterpStencil(RectMDArray<Stencil>){};

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
    operator()(const RectMDArray<T>& a_phi,
      const Box& a_bx) const 
    {
      auto t = std::make_tuple(this, &a_phi, &a_bx);
      return t;
    }

  /// Applies a Stencil to a_phi and stores it in a_lofphi.
    static void apply(const Stencil<T>& a_stencil,
      const RectMDArray<T>& a_phi,
      RectMDArray<T>& a_lofPhi,
      const Box& a_bx);


  /// Returns the coefficients of the Stencil.
  const vector<T>&  getCoefs() const
  { 
    return m_coef; 
  }

  /// Returns the offsets of the Stencil.
  const vector<Point>& getOffsets() const 
  {
    return m_offsets;
  }

  /// Alternative printing function to stencilDump(). 
  void print() const
  {
    cout << "Coeficients and Shifts: " << endl;
    for (int ii = 0; ii < m_coef.size(); ii++) {
      cout << m_coef[ii] << ": ";
      m_offsets[ii].print();
    }
    cout << "srcRefratio: ";
    m_srcRefratio.print();
    cout << "destRefratio: ";
    m_destRefratio.print();
    cout << "destShift: ";
    m_destShift.print();
    cout << endl;
  }

  private:
  vector<T> m_coef;///< Coefficients of the Stencil
  vector<Point> m_offsets; ///< Offsets associated with the Stencil. Each offset corresponds to a coefficient in m_coef.
  Point m_srcRefratio; 
  Point m_destRefratio;
  Point m_destShift;
  Stencil<T>(vector<T > a_vecT, vector<Point > a_vecPt,
   Point a_destRefratio=getOnes(),
   Point a_destShift=getZeros(),
   Point a_srcRefratio=getOnes());

};

#include "StencilImplem.H"

/// Defines the product of a coefficient of type T and a shift as a Stencil.
/**
  This is a shortcut for creating Stencils simply using the syntax:\n
  @code
  Stencil<double> S = 3.7*shift 
  @endcode
  Where shift is a valid shift object.
*/
template <class T> Stencil<T> 
  operator*(T a_coef, Shift a_shift)
  {return Stencil<T>(pair<Shift,T>(a_shift,a_coef));};

/// Replacement operator used for Stencil application to a RectMDArray.
/**
  This operator is used for replacing the data of a_lofPhi within a box B with the output of the Stencil operation. Data not within B is unchanged.
*/
template <class T>
  RectMDArray<T>& operator|=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token)
  {
    const Box& b=*(std::get<2>(a_token));
    forall<T>(a_lofPhi,[](T& t){ t=0; }, b);
    Stencil<T>::apply(*(std::get<0>(a_token)), *(std::get<1>(a_token)), a_lofPhi, b);
    return a_lofPhi;
  }

/// Increment operator used for Stencil application to a RectMDArray.
/**
  This operator is used for summing the data of a_lofPhi within a box B with the output of the Stencil operation. Data not within B is unchanged.  
*/
template <class T>
  RectMDArray<T>& operator+=(RectMDArray<T>& a_lofPhi, const std::tuple<const Stencil<T>*, const RectMDArray<T>*,const Box* >& a_token)
  {
    const Box& b=*(std::get<2>(a_token));
    // assert(a_lofPhi.getBox().contains(b));
    Stencil<T>::apply(*(std::get<0>(a_token)), *(std::get<1>(a_token)), a_lofPhi, b);
    return a_lofPhi;
  }

/// Stencil application function.
/**
  This function is called by the += and |= operators. See the documentation of these operators in the Stencil.H file for more information.
  \bug Timer code does not appear to be working and has been commented out.
*/
template <class T>
void Stencil<T>::apply(const Stencil<T>& a_stencil,
                       const RectMDArray<T>& a_phi,
                       RectMDArray<T>& a_lofPhi,
                       const Box& a_bx)
{
  CH_TIMERS("Stencil::apply");
  //CH_TIMER("innerloop",t1);
  const vector<T> & coef = a_stencil.m_coef;
  const vector<Point> & offsets = a_stencil.m_offsets;
  const Point& srcRefratio = a_stencil.m_srcRefratio;
  const Point& destRefratio = a_stencil.m_destRefratio;
  const Point& destShift = a_stencil.m_destShift;

  int ncoef = coef.size();
  int tuple[DIM];

  int nptsDst = a_bx.getHighCorner()[0] - a_bx.getLowCorner()[0]+1;

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
  //CH_START(t1);
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

  double* phi_ptr = &a_phi[0];
  // double* lofphi_ptr = &a_lofPhi[0];
  double* lofphi_ptr = &(a_lofPhi.index(0));
  for (int kperp = 0; kperp < size; kperp++)
    {
      Point pt = cross.getPoint(kperp);
      int kbasel = bxl.getIndex(pt*destRefratio + destShift); 
      int kbasep = bxp.getIndex(pt*srcRefratio);
      for (int isten = 0;isten < srcOffset.size(); isten++)
        {
          //loop through each point in destination box
          for (int k0 = 0;k0 < nptsDst ; k0++)              
            {
              int offset =  srcOffset[isten];   
              int k0dest = k0*desdisp;

              int k0src = k0*srcdisp; 
              double coefpt = coef[isten];
              double phival = phi_ptr[kbasep + k0src + offset];

              lofphi_ptr[kbasel + k0dest] =  lofphi_ptr[kbasel + k0dest] + coefpt*phival;
              //a_lofPhi[kbasel + k0dest] =  lofphi + coefpt*phival;
            }
        }
    }
  //CH_STOP(t1);
}

/// Applies a Stencil to a given component of a source RectMDArray and stores it in a given component of another RectMDArray.
template <class T, int SRCCOMP, int DSTCOMP>
  void componentApply(const Stencil<T>             & a_stencil,
    const RectMDArray<T, SRCCOMP>& a_phi,
    RectMDArray<      T, DSTCOMP>& a_lofPhi,
    const Box                    & a_bx,
    const int                    & a_srccomp,
    const int                    & a_dstcomp)
  {
    CH_TIMERS("applyOpSlow");
    const vector<T> & coef = a_stencil.getCoefs();
    const vector<Point> & offsets = a_stencil.getOffsets();
    assert(a_dstcomp >= 0);
    assert(a_dstcomp < DSTCOMP);

    assert(a_srccomp >= 0);
    assert(a_srccomp < SRCCOMP);
    
    assert(coef.size() == offsets.size());
    int ncoef = coef.size();
    for ( Point dstpt = a_bx.getLowCorner();a_bx.notDone(dstpt);a_bx.increment(dstpt))
    {
      for(int isten = 0; isten < ncoef; isten++)
      {
        Point srcpt   = dstpt + offsets[isten];
        double weight =     coef[isten];

        double lofphi = a_lofPhi(dstpt, a_dstcomp);
        double phival =    a_phi(srcpt, a_srccomp);
        a_lofPhi(dstpt,a_dstcomp) =  lofphi + weight*phival;
      }
    }
  }



#endif
