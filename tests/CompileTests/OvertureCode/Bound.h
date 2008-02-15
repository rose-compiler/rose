#ifndef BOUND_H
#define BOUND_H "bound.h"

#include "OvertureTypes.h"
#include "Fraction.h"

// Define a bound type as either a fraction or a real number

enum boundType{ realNumber, fraction, null };

class Bound{
 protected:
  boundType bt;
  real x;
  Fraction f;
 public:
  Bound( ){ bt=null; }
  Bound( const real x0 ){ bt=realNumber; x=x0; }
  Bound( const int i ){ bt=fraction; f=Fraction(i,1); }
  Bound( const Fraction f0 ){ bt=fraction; f=f0; }
  ~Bound(){};
  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

//  ---here are the access routines to set/get values
  inline void set( const real value );
  inline void set( const int value );
  inline void set( const int n, const int d );
  void get( boundType & bndType, real & rvalue, Fraction & fvalue ) const;

  bool isFinite();
  operator real();
  
//  ---here are routines to define arithmetic and relational operators
  friend Bound operator + ( const Bound b1, const Bound b2 );
  friend Bound operator - ( const Bound b1, const Bound b2 );
  friend Bound operator * ( const Bound b1, const Bound b2 );
  friend Bound operator / ( const Bound b1, const Bound b2 );
  friend int operator <=( const Bound b1, const Bound b2 );
  friend int operator < ( const Bound b1, const Bound b2 );
  friend int operator >=( const Bound b1, const Bound b2 );
  friend int operator > ( const Bound b1, const Bound b2 );
  friend int operator ==( const Bound b1, const Bound b2 );
  friend ostream& operator<< ( ostream&, const Bound& );
};


inline void Bound::set( const real value ){ bt=realNumber; x=value;}
inline void Bound::set( const int value ){ bt=fraction; f=Fraction( value ); }
inline void Bound::set( const int n, const int d ){ bt=fraction; f=Fraction(n,d); }

#endif // BOUND_H

