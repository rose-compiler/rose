#ifndef FRACTION_H
#define FRACTION_H "fraction.h"

#include "OvertureTypes.h"
#include "aString.H"         // aString Class
class GenericDataBase;     

// Here we define a fration to be a pair of integers called
// numerator and denominator. 
// Examples:
//   (1,1)==(2,2) : "one"
//   (1,0) : infinity
//   (-1,0) : -infinity
//   (1,0) <= (2,0),   (5,1) <= (1,0) 
//
class Fraction
{
 protected:
  int numerator;
  int denominator;
 public:
  Fraction(){};
  Fraction( const int n, const int d=1 ){ numerator=n; denominator=d; };
  ~Fraction(){};

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Fraction operator + ( const Fraction & f1 ) const;  // should these be return by reference?
  Fraction operator - ( const Fraction & f1 ) const;  // maybe not, see Stroustrup
  Fraction operator - ( ) const;
  Fraction operator * ( const Fraction & f1 ) const;  
  Fraction operator / ( const Fraction & f1 ) const;
  int operator <= ( const Fraction & f1 ) const;
  int operator <  ( const Fraction & f1 ) const;
  int operator >= ( const Fraction & f1 ) const;
  int operator >  ( const Fraction & f1 ) const;
  int operator == ( const Fraction & f1 ) const;
  int operator <= ( const real x ) const;
  int operator <  ( const real x ) const;
  int operator >= ( const real x ) const;
  int operator >  ( const real x ) const;
  int operator == ( const real x ) const;

// We have to define all these member functions because we do not know
// how to construct a fraction from a real number!
  friend int operator <= ( const real, const Fraction );
  friend int operator <  ( const real, const Fraction );
  friend int operator >= ( const real, const Fraction );
  friend int operator >  ( const real, const Fraction );
  friend int operator == ( const real, const Fraction );
  friend real operator + ( const real, const Fraction );  // real+Fraction=real 
  friend real operator + ( const Fraction , const real ); 
  friend real operator - ( const real, const Fraction );  // real-Fraction=real 
  friend real operator - ( const Fraction , const real ); 
  friend real operator * ( const real, const Fraction );  // real*Fraction=real 
  friend real operator * ( const Fraction , const real ); 
  friend real operator / ( const real, const Fraction );  // real/Fraction=real 
  friend real operator / ( const Fraction , const real ); 

  inline void setNumerator( const int n );
  inline void setDenominator( const int d );
  inline int getNumerator() const;
  inline int getDenominator() const;

  friend ostream& operator<< ( ostream&, const Fraction& );
};

inline void Fraction::setNumerator( const int n ){ numerator=n; }
inline void Fraction::setDenominator( const int d ){ denominator=d; }
inline int Fraction::getNumerator() const { return numerator; }
inline int Fraction::getDenominator() const { return denominator; }

#endif // FRACTION_H
