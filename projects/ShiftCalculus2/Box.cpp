# include "Box.H"
#include <cassert>
///

bool reportMemory = false;
unsigned long long int memory=0;

Box::Box()
{
  m_lowCorner = getZeros();
  m_highCorner= getOnes();
  m_highCorner *= -1;
  m_size = -1;
};
///
Box::Box(const Box& a_box)
{
  m_lowCorner = a_box.m_lowCorner;
  m_highCorner = a_box.m_highCorner;
  m_size = a_box.m_size;
};
///
void
Box::recomputeSize()
{
  m_size = 1;
  for(int idir = 0; idir < DIM; idir++)
    {
      m_size *= size(idir);
    }
}
Box::Box(const Point& a_lowCorner,const Point& a_highCorner)
{
  m_lowCorner = a_lowCorner;
  m_highCorner = a_highCorner;
  recomputeSize();
};
//Box returns a new box, should have put const to make clear.
Box Box::operator&(const Box& a_rightBox) const
{
  int newLo[DIM];
  int newHi[DIM];
  for (int i = 0 ; i < DIM; i++)
    {
      newLo[i] = m_lowCorner[i];
      if (m_lowCorner[i] < a_rightBox.m_lowCorner[i])
        {
          newLo[i] = a_rightBox.m_lowCorner[i];
        }
      newHi[i] = m_highCorner[i];
      if (m_highCorner[i] > a_rightBox.m_highCorner[i])
        {
          newHi[i] = a_rightBox.m_highCorner[i];
        }
    }
  Point newLowCorner(newLo);
  Point newHighCorner(newHi); 
  for (int dir = 0; dir < DIM; dir++)
    {
      if (newHi[dir] < newLo[dir])
        {
          Box ret0;
          return ret0;
        }
    }
  Box ret(newLowCorner,newHighCorner);
  return ret;
};

void Box::operator&=(const Box& a_rightBox) 
{
  Box retval = *this & a_rightBox;
  *this = retval;
};
Box Box::shift(int a_direction, int a_offset) const
{
  Box returnBox = Box(*this);
  returnBox.m_lowCorner += getUnitv(a_direction)*a_offset;
  returnBox.m_highCorner += getUnitv(a_direction)*a_offset;
  return returnBox;
};
Box Box::shift(const Point& a_pt) const
{
  Box returnBox = Box(*this);
  returnBox.m_lowCorner += a_pt;
  returnBox.m_highCorner += a_pt;
  return returnBox;
};
Box Box::grow(int a_offset) const
{
  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo -= getOnes()*a_offset;
  hi += getOnes()*a_offset;
  Box returnBox(lo, hi);

  return returnBox;
};
Box Box::grow(const Point& a_offset) const
{
  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo -= a_offset;
  hi += a_offset;
  Box returnBox(lo, hi);

  return returnBox;
};
Box Box::coarsen(int a_nref) const
{
  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo /= a_nref;
  hi /= a_nref;
  Box returnBox(lo, hi);

  return returnBox;
};
Box Box::coarsen(const Point& a_pt) const
{
  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo /= a_pt;
  hi /= a_pt;
  Box returnBox(lo, hi);

  return returnBox;
};
Box Box::refine(int a_nref) const
{

  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo *= a_nref;
  hi += getOnes();
  hi *= a_nref;
  hi -= getOnes();
  Box returnBox(lo, hi);

  return returnBox;
};
Box Box::refineCC(const Point& a_pt) const
{
  return refine(a_pt);
};
Box Box::refineCC(int a_nref) const
{
  return refine(a_nref);
};
Box Box::refine(const Point& a_pt) const
{
  Point lo = m_lowCorner;
  Point hi = m_highCorner;
  lo *= a_pt;
  hi += getOnes();
  hi *= a_pt;
  hi -= getOnes();
  Box returnBox(lo, hi);

  return returnBox;
};

void Box::increment(Point& a_pt) const
{
  Point current = a_pt;
  assert(DIM <= 3);

  current[0]++;
#if DIM >= 2
  if (current[0] > m_highCorner[0])
    {
      current[0] = m_lowCorner[0];
      current[1]++;
#if DIM >= 3
      if (current[1] > m_highCorner[1])
        {
          current[1] = m_lowCorner[1];
          current[2]++;
        }
#endif
    }
#endif
  a_pt = current;

};


Point Box::getPoint(unsigned int k) const
{
  assert(k >= 0);
  int tuple[DIM];
  for (unsigned char i = 0;i < DIM; i++)
    {      
      int factor = (m_highCorner[i] - m_lowCorner[i] + 1);
      int kred = k%factor;
      tuple[i] = kred + m_lowCorner[i];
      k = (k - kred)/factor;
    }
  Point pt(tuple);
  return pt;
};

bool Box::contains(const Point& a_pt) const
{
  bool retval = true;
  for(int idir = 0; idir < DIM; idir++)
    {
      if(a_pt[idir] < m_lowCorner[idir])
        {
          retval = false;
        }
      if(a_pt[idir] > m_highCorner[idir])
        {
          retval = false;
        }
    }
  return retval;
}

bool Box::hasPoint(const Point& a_point) const
{
  //check that point is in between lowCorner and highCorner
  for (int i = 0; i < DIM; i++)
    {
      if (a_point[i] > m_highCorner[i])
	{
	  return false;
	}
      else if (a_point[i] < m_lowCorner[i])
	{
	  return false;
	}
    }
  return true;
}
void Box::print() const 
{
  std::cout<<*this<<std::endl;
}
ostream& operator<<(ostream& os, const Box& a_box)
{
  os << "[low Corner = " ;
  for (int k = 0;k < DIM; k++)
    {
      os << a_box.getLowCorner()[k]<< " ";
    }
  os << " high Corner = " ;
  for (int k = 0;k < DIM; k++)
    {
      os << a_box.getHighCorner()[k] << " ";
    }
  os<<" size="<<a_box.sizeOf();
  os<<"]";
  return os;
}

Point Box::mod(const Point& a_pt) const
{
  int tuple[DIM];
  for (int i = 0;i< DIM; i++)
    {
      int dl = m_highCorner[i] - m_lowCorner[i] + 1;
      tuple[i] = mymod(a_pt[i],dl) + m_lowCorner[i];
    }
  return Point(tuple);
}
  
