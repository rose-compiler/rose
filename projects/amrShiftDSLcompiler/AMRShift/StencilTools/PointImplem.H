#include <assert.h>

inline Point getOnes()
{
  int ones[DIM];
  for (int k = 0 ; k < DIM; k++)
    {
      ones[k] = 1;
    }
  return Point(ones);
};
inline Point getZeros()
{
  int zeros[DIM];
  for (int k = 0 ; k < DIM; k++)
    {
      zeros[k] = 0;
    }
  return Point(zeros);
};
inline Point getBasisV(int idir)
{
  Point retval = getZeros();
  retval[idir] = 1;
  return retval;
}
// Point getUnitv(int a_dir)
// {
//   assert((a_dir < DIM) && (a_dir >= 0));
//   int unitv[DIM];
//   for (int k = 0 ; k < DIM; k++)
//     {
//       unitv[k] = 0;
//     }
//  unitv[a_dir] = 1;
//  return Point(unitv);
// };
Point::Point(){};
Point::Point(const int a_tuple[DIM])
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j]=a_tuple[j];
    }
};
Point::Point(const array<int,DIM> a_tuple)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j]=a_tuple[j];
    }
};
Point::Point(const Point& a_pt)
{
  *this = a_pt;
};

/** This is just an ordering. It is used to add Points to maps etc. Don't use it to actually compare Points. */
bool Point::operator<(const Point& a_rhs) const
{
  for (int i = 0; i < DIM; i++)
    {
      if (m_tuple[i] < a_rhs[i])
	{
	  return true;
	}
      else if (m_tuple[i] > a_rhs[i])
	{
	  return false;
	}
    }
  return false;
}

Point Point::operator+(const Point& a_rhsPoint) const
{
  int tuple[DIM];
  for (int j = 0; j < DIM; j++)
    {
      tuple[j] = m_tuple[j]+a_rhsPoint.m_tuple[j]; //a_tuple
    }
  Point ret(tuple);
  return ret;
};
Point Point::operator-(const Point& a_rhsPoint) const
{
  int tuple[DIM];
  for (int j = 0; j < DIM; j++)
    {
      tuple[j] = m_tuple[j]-a_rhsPoint.m_tuple[j]; 
    }
  Point ret(tuple);
  return ret;
};
Point Point::operator*(const Point& a_rhsPoint) const
{
  int tuple[DIM];
  for (int j = 0; j < DIM; j++)
    {
      tuple[j] = m_tuple[j]*a_rhsPoint.m_tuple[j]; //a_tuple
    }
  Point ret(tuple);
  return ret;
}; 
Point Point::operator/(const Point& a_rhsPoint) const
{
  int tuple[DIM];
  for (int j = 0; j < DIM; j++)
    {
      assert(a_rhsPoint[j]!=0);
      if (m_tuple[j]*a_rhsPoint[j] >= 0)
        {
          tuple[j] = m_tuple[j]/a_rhsPoint[j];
        }
      else
        {
          tuple[j] = -(-m_tuple[j]/a_rhsPoint[j]);
        }
    }
  Point ret(tuple);
  return ret;

};
Point Point::operator*(int a_nref) const
{
  int tuple[DIM];
  for (int j = 0; j < DIM; j++)
    {
      tuple[j] = m_tuple[j]*a_nref;
    }
  Point ret(tuple);
  return ret;
};    
Point Point::operator/(int a_nref) const
{
  int tuple[DIM];
  assert(a_nref != 0);
  for (int j = 0; j < DIM; j++)
    {
      
      if (m_tuple[j]*a_nref >= 0)
        {
          tuple[j] = m_tuple[j]/a_nref;
        }
      else
        {
          tuple[j] = -(-m_tuple[j]/a_nref);
        }
    }
  Point ret(tuple);
  return ret;
};
void Point::operator+=(const Point& a_rhsPoint)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]+a_rhsPoint.m_tuple[j]; 
    }
};
void Point::operator-=(const Point& a_rhsPoint)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]-a_rhsPoint.m_tuple[j]; 
    }
};
void Point::operator*=(const Point& a_rhsPoint)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]*a_rhsPoint.m_tuple[j]; 
    }
};
void Point::operator/=(const Point& a_rhsPoint)
{
  for (int j = 0; j < DIM; j++)
    {
      assert(a_rhsPoint[j]!=0);
      if (m_tuple[j]*a_rhsPoint[j] >= 0)
        {
          m_tuple[j] = m_tuple[j]/a_rhsPoint[j];
        }
      else
        {
          m_tuple[j] = -(-m_tuple[j]/a_rhsPoint[j]);
        }
    }
};
void Point::operator+=(int a_nref)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]+a_nref; 
    }
};
void Point::operator-=(int a_nref)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]-a_nref; 
    }
};
void Point::operator*=(int a_nref)
{
  for (int j = 0; j < DIM; j++)
    {
      m_tuple[j] = m_tuple[j]*a_nref; 
    }
};
void Point::operator/=(int a_nref)
{
  assert(a_nref!=0);
  for (int j = 0; j < DIM; j++)
    {
      
      if (m_tuple[j]*a_nref >= 0)
        {
          m_tuple[j] = m_tuple[j]/a_nref;
        }
      else
        {
          m_tuple[j] = -(-m_tuple[j]/a_nref);
        }
    }
};
bool Point::operator==(const Point& a_rhsPoint) const 
{
  bool ret = true;
  for (int j = 0; j < DIM; j++)
    {
      if (m_tuple[j] != a_rhsPoint[j]) ret=false;
    }
  return ret;
};
bool Point::operator!=(const Point& a_rhsPoint) const 
{
  return !(*this == a_rhsPoint);
};
// const int& Point::operator[](const int& a_index) const
// {
//   assert(a_index < DIM);
//   assert(a_index >= 0);
//   return m_tuple[a_index];
// };
