#include "CH_Timer.H"
#include "Box.H"
template <class TCoef> 
Stencil<TCoef>::Stencil(){};
template <class TCoef> 
Stencil<TCoef>::Stencil(pair<Shift,TCoef> a_pair,
                                   Point a_destRefratio,
                                   Shift a_destShift,
                                   Point a_srcRefratio)
{
  m_coef.push_back(a_pair.second);
  m_offsets.push_back(a_pair.first.m_shift);
  m_destRefratio = a_destRefratio;
  m_srcRefratio = a_srcRefratio;
  m_destShift = a_destShift.m_shift;
};

template <class TCoef> 
Stencil<TCoef>::Stencil(vector<TCoef> a_vecT,
                        vector<Point> a_vecPt,
                        Point a_destRefratio,
                        Point a_destShift,
                        Point a_srcRefratio)
{
  m_coef = a_vecT;
  m_offsets = a_vecPt;
  m_destRefratio = a_destRefratio;
  m_srcRefratio = a_srcRefratio;
  m_destShift = a_destShift;
 
};

template <class TCoef> 
Stencil<TCoef> Stencil<TCoef>::operator+
(const Stencil<TCoef> a_stencil) const
{
  if ((m_destShift != a_stencil.m_destShift) ||
      (m_destRefratio != a_stencil.m_destRefratio))
    {
      cout << "error - trying to add two stencils with different"
           << " LHS shifts / ratios" << endl;
      abort();
    }
  vector<TCoef> newCoefs = a_stencil.m_coef;
  vector<Point> newOffsets = a_stencil.m_offsets;
  for (int l1 = 0 ; l1 < m_coef.size();l1++)
    {
      Point loc = m_offsets[l1];
      bool isThere = false;
      int locThere;
      for (int l2 = 0;l2 < a_stencil.m_coef.size();l2++)
        {
          if (a_stencil.m_offsets[l2] == loc)
            {
              isThere = true;
              locThere = l2;
            }
        }
      if (isThere) 
        {
          newCoefs[locThere] += m_coef[l1];
        }
      else
        {
          newCoefs.push_back(m_coef[l1]);
          newOffsets.push_back(m_offsets[l1]);
        }
    }
  return Stencil(newCoefs,newOffsets,m_destRefratio,m_destShift,m_srcRefratio);
};

template <class TCoef> 
Stencil<TCoef> Stencil<TCoef>::operator*
(const Stencil<TCoef> a_stencil) const
{
  cout << "warning - this is wrong. Must fix." << endl;
  abort();
  vector<TCoef> newCoefs = a_stencil.m_coef;
  vector<Point> newOffsets = a_stencil.m_offsets;
  for (int l1 = 0 ; l1 < m_coef.size();l1++)
    {
      Point loc = m_offsets[l1];
      int locThere;
      bool isThere = false;
      for (int l2 = 0;l2 < a_stencil.m_coef.size();l2++)
        {
          if (a_stencil.m_offsets[l2] == loc)
            {
              isThere = true;
              locThere = l2;
            }
        }
      if (isThere) 
        {
          newCoefs[locThere] += m_coef[l1];
        }
      else
        {
          newCoefs.push_back(m_coef[l1]);
          newOffsets.push_back(m_offsets[l1]);
        }
    }
  return Stencil(newCoefs,newOffsets,m_destRefratio,m_destShift,m_srcRefratio);
};

template <class TCoef>
void Stencil<TCoef>::operator*=(const TCoef& a_coef)
{
  for (int l = 0; l < m_coef.size(); l++)
    {
      m_coef[l]*=a_coef;
    }
};

template <class TCoef>
void Stencil<TCoef>::stencilDump() const
{
  Stencil<TCoef> tmp = *this;
  cout << "coefs and offsets :" << endl ;
  for (int k = 0; k < tmp.m_coef.size(); k++)
    {
      cout << k << " , "<< m_coef[k] << " , ( " ;
      for (int dir = 0;dir < DIM; dir++)
        {
          cout << tmp.m_offsets[k][dir] << " ";
        }
      cout << ") ; " ;
      cout << endl;
    }
  cout << "sourceRef, destRef, and destShift: " << m_srcRefratio << m_destRefratio << m_destShift << endl ;
  /*
  for (int dir = 0;dir < DIM; dir++)
    {
      cout << tmp.m_srcRefratio[dir] << "  ";
    }
  cout << " , " ;
  for (int dir = 0;dir < DIM; dir++)
    {
      cout << tmp.m_destRefratio[dir] << "  ";
    }
  cout << " , " ;
  for (int dir = 0;dir < DIM; dir++)
    {
      cout << tmp.m_destShift[dir] << " ";
    }
  cout << endl;
  */
  cout << endl;
};
/*template <class TSrc, class TDest>
void applyOp(const Stencil<TCoef> a_stencil,
        const RectMDArray<TDest >& a_LOfPhi,
        const RectMDArray<TSrc >& a_phi,
        const Box& a_bx) */

