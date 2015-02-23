#ifndef _BOX_H_
#define _BOX_H_
#include "SPACE.H"
#include "Point.H"
#include <iostream>
extern bool reportMemory;
extern unsigned long long int memory;

class Box
{
public:
  Box();
  /// constructor for nontrivial Box. 
  Box(const Point& a_lowCorner,const Point& a_highCorner);
  /// copy constructor.
  Box(const Box& a_Box);
  ///  Computes the intersection of the Box with rightBox.  Box AND operation. 
  Box operator&(const Box& a_rightBox) const;
  ///does intersection in place
  void operator&=(const Box& a_rightBox);

  /// Computes shifted Box by a_offset in direction a_direction.
  /**
     does not change *this. returns the shifted box
   */
  Box shift(int a_direction, int a_offset) const;
  /// Computes shifted Box by a_pt.
  /**
     does not change *this. returns the shifted box
   */
  Box shift(const Point& a_pt) const;
  /// Grow in all of the coordinate directions by a_numpoints.
  /**
     does not change *this. returns the grown box
   */
  Box grow(int a_numpoints) const;

  /// Grow in each coordinate direction by an amount given by the component of a_pt.
  /**
     does not change *this. returns the grown box
   */
  Box grow(const Point& a_pt) const;

  /// coarsen in all of the coordinate directions by a_numpoints.
  /**
     does not change *this. returns the coarsened box
   */
  Box coarsen(int a_numpoints) const;
  /// coarsen in each coordinate direction by an amount given by the component of a_pt.
  Box coarsen(const Point& a_pt) const;
  /// refine in all of the coordinate directions by a_numpoints.
  Box refine(int a_numpoints) const;
  /// refine in each coordinate direction by an amount given by the component of a_pt.
  Box refine(const Point& a_pt) const;
  Box refineCC(const Point& a_pt) const;
  Box refineCC(int a_nref) const;
  /// Access functions for low, high bounds of Box.
  inline const Point& getLowCorner() const {return m_lowCorner;};
  inline const Point& getHighCorner() const {return m_highCorner;}; 

  inline Point& getLowCorner()  {return m_lowCorner;};
  inline Point& getHighCorner() {return m_highCorner;}; 

  /// Computes a linear index corresponding to the input array. 
  int linearIndex(const Point& a_pt) const;
  /// Computes the size of the Box.
  const int& sizeOf() const{return m_size;};
  /// When are two boxes equal.
  bool operator==(const Box& a_rhsBox) const
  {return ((m_lowCorner == a_rhsBox.m_lowCorner) 
           && (m_highCorner == a_rhsBox.m_highCorner));};
  /// Contains the Point a_pt.
  bool contains(const Point& a_pt) const;
  bool contains(const Box& a_rhs) const{
    return this->contains(a_rhs.m_lowCorner) && this->contains(a_rhs.m_highCorner);
  }
  bool contains(const Box& a_rhs, const Box& a_extent)
  {
    return this->contains(a_rhs.m_lowCorner +a_extent.m_lowCorner) && 
      this->contains(a_rhs.m_highCorner+a_extent.m_highCorner);
  }
  bool hasPoint(const Point& a_point) const;
  /// Finds periodic image of input that is contained in the Box.
  Point mod(const Point& a_pt) const;
  /// Get linear index of a Point in a Box.
  inline unsigned int  getIndex(const Point& a_pt) const;
  inline bool notDone(const Point& a_pt) const;

  unsigned int size(unsigned char a_dim) const
  {
    return m_highCorner[a_dim]-m_lowCorner[a_dim] + 1;
  }
  /// iteration through the points in a Box. a_pt is incremented to the next point
  /// in the Box.
  void increment(Point& a_pt) const;
  /// Get Point corresponding to a linear index in [0, ... sizeOf()-1] inside the Box
  Point getPoint(unsigned int k) const;
  void print() const; 

  bool isEmpty() const 
  {bool ret = (m_size < 1); return ret;};
private:  
  void recomputeSize();
  Point m_lowCorner,m_highCorner;  
  int m_size;
};

ostream& operator<<(ostream& os, const Box& a_box);
//////// Implementation details


inline  bool Box::notDone(const Point& a_pt) const
{
  bool retval = true;
  for(int idir = 0; idir < DIM; idir++)
    {
      retval = retval && (a_pt[idir] <= m_highCorner[idir]);
    }
  return retval;
}

inline unsigned int Box::getIndex(const Point& a_pt) const
{
  unsigned int factor = 1;
  unsigned int linIndex = a_pt[0] - m_lowCorner[0];
  for (unsigned char i = 1;i < DIM;i++)
    { 
      factor = factor*(m_highCorner[i-1] - m_lowCorner[i-1]+1);
      linIndex = linIndex + (a_pt[i] - m_lowCorner[i])*factor;
    }
  return linIndex;
}
inline int mymod(int i,int j) 
{
  if (i < 0)
    {
      int k = (-1-i)%j;
      return j-1-k;
    }
  else
    {
      return i%j;
    }
}


#endif
