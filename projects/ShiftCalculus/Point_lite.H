#ifndef POINT_H
#define POINT_H //what is this for ?

// #include <assert.h>
// #include <iostream>

//#include <array>

// DQ: Forward reference required (for reference to Shift below).
class Shift;

using namespace std;
class Point
{
  public:
  /// Default Constructor
  Point();
  /// Constructor
  Point(const int a_tuple[DIM]);
  //  inline Point(const array<int,DIM> a_tuple);
  /// Copy constructor
  Point(const Point& a_pt);
  ///Returns true if the left hand side is less than the right hand side.
  bool operator<(const Point& a_rhs) const;
  /// Adds two points
  Point operator+(const Point& a_rhsPoint) const;
  /// Subtracts two points
  Point operator-(const Point& a_rhsPoint) const;
  /// division by an integer - rounds down.
  Point operator/(int a_nref) const;
  /// Componentwise division by a Point - rounds down.
  Point operator/(const Point& a_pt) const;
  /// multiplication by an integer.
  Point operator*(int a_nref) const;
  /// Componentwise multiplication by a Point.
  Point operator*(const Point& a_pt) const;
  /// Update in place arithmetic operations.
  void operator*=(const Point& a_pt);
  void operator+=(const Point& a_pt);
  void operator-=(const Point& a_pt);
  void operator/=(const Point& a_pt);
   /// Update in place aritmetic operations. Adds integer to every component.
  void operator*=(int a_n);
  void operator+=(int a_n);
  void operator-=(int a_n);
  void operator/=(int a_n);
  /// are two points equal.
  bool operator==(const Point& a_pt) const;
  /// are two points equal.
  bool operator!=(const Point& a_pt) const;

// DQ: Added assignment operator for Point class (for when rhs is type Shift).
  Point operator= (const Shift & x);

  /// Get a component of a point.
  const int&  operator[](const int& a_index) const;
  void print() const;
 private:
  int m_tuple[DIM];
};

Point getZeros();
Point getOnes();
Point getUnitv(int a_dir);

// DQ: We don't need the implementations for analysis.
// #include "PointImplem.H"

#endif
