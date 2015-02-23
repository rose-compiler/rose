#ifndef POINT_H
#define POINT_H //what is this for ?
#include "SPACE.H"
#include <assert.h>
#include <iostream>
#include <array>


using namespace std;
class Point
{
public:
  /// Default Constructor
  inline Point();
  /// Constructor
  inline Point(const int a_tuple[DIM]);
  inline Point(const array<int,DIM> a_tuple);
  /// Copy constructor
  inline Point(const Point& a_pt);
  ///Returns true if the left hand side is less than the right hand side.
  /** 
      This is just an ordering. It is used to add Points to maps etc. Don't use it to actually compare Points. 
  */
  inline bool operator<(const Point& a_rhs) const;

  /// Adds two points
  inline Point operator+(const Point& a_rhsPoint) const;
  /// Subtracts two points
  inline Point operator-(const Point& a_rhsPoint) const;
  /// division by an integer - rounds down.
  inline Point operator/(int a_nref) const;
  /// Componentwise division by a Point - rounds down.
  inline Point operator/(const Point& a_pt) const;
  /// multiplication by an integer.
  inline Point operator*(int a_nref) const;
  /// Componentwise multiplication by a Point.
  inline Point operator*(const Point& a_pt) const;
  /// Update in place arithmetic operations.
  inline void operator*=(const Point& a_pt);
  inline void operator+=(const Point& a_pt);
  inline void operator-=(const Point& a_pt);
  inline void operator/=(const Point& a_pt);
  /// Update in place aritmetic operations. Adds integer to every component.
  inline void operator*=(int a_n);
  inline void operator+=(int a_n);
  inline void operator-=(int a_n);
  inline void operator/=(int a_n);
  /// are two points equal.
  inline bool operator==(const Point& a_pt) const;
  /// are two points equal.
  inline bool operator!=(const Point& a_pt) const;
  /// Get a component of a point.
  inline const int&  operator[](const int& a_index) const
  {
    //assert(a_index < DIM);
    //assert(a_index >= 0);
    return m_tuple[a_index];
  };

  inline int&  operator[](const int& a_index)
  {
    //assert(a_index < DIM);
    //assert(a_index >= 0);
    return m_tuple[a_index];
  };
  inline void print() const
  {
    for (int dir = 0; dir < DIM ; dir++)
      {
        cout << m_tuple[dir] << " " ;
      }
    cout << endl;
  };

  inline void print2() const
  {
    for (int dir = 0; dir < DIM ; dir++)
      {
        cout << m_tuple[dir] << " " ;
      }
  };

private:
  int m_tuple[DIM];
};
inline Point getBasisV(int idir);
inline Point getZeros();
inline Point getOnes();
inline Point getUnitv(int a_dir)
{
  assert((a_dir < DIM) && (a_dir >= 0));
  int unitv[DIM];
  for (int k = 0 ; k < DIM; k++)
    {
      unitv[k] = 0;
    }
  unitv[a_dir] = 1;
  return Point(unitv);
};
#include "PointImplem.H"
#endif
