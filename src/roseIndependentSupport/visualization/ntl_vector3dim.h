/******************************************************************************
 *
 * Basic vector class used everywhere.
 * Lots of inline functions ...
 * Typedefs for ntlVec3d, ntlVec3i ... further below
 *
 *****************************************************************************/


#ifndef NTL_VECTOR3DIM_HH
#define NTL_VECTOR3DIM_HH


#if __GNUC__ == 3
#include <iostream>
#else
#include <iostream.h>
#endif

//#include <values.h>
#include <math.h>

#define ABS(x) (x>0 ? (x) : (-x) )

/* The minimal vector length
   In order to be able to discriminate floating point values near zero, and
   to be sure not to fail a comparison because of roundoff errors, use this
   value as a threshold.  */
#define VECTOR_EPSILON (1e-10)

/* init vectors with zero? */
#define VECTOR_INIT_ZERO true

/* convert a float to double vector */
#define Vecd2f(b) (ntlVec3f( b[0],b[1],b[2] ))
#define Vecf2d(b) (ntlVec3d( b[0],b[1],b[2] ))

// The basic vector class
template<class Scalar>
class ntlVector3Dim
{
public:
  // Constructor
  inline ntlVector3Dim(void );
  // Copy-Constructor
  inline ntlVector3Dim(const ntlVector3Dim<Scalar> &v );
  // construct a vector from one Scalar
  inline ntlVector3Dim(Scalar);
  // construct a vector from three Scalars
  inline ntlVector3Dim(Scalar, Scalar, Scalar);

  // Assignment operator
  inline const ntlVector3Dim<Scalar>& operator=  (const ntlVector3Dim<Scalar>& v);
  // Assignment operator
  inline const ntlVector3Dim<Scalar>& operator=  (Scalar s);
  // Assign and add operator
  inline const ntlVector3Dim<Scalar>& operator+= (const ntlVector3Dim<Scalar>& v);
  // Assign and add operator
  inline const ntlVector3Dim<Scalar>& operator+= (Scalar s);
  // Assign and sub operator
  inline const ntlVector3Dim<Scalar>& operator-= (const ntlVector3Dim<Scalar>& v);
  // Assign and sub operator
  inline const ntlVector3Dim<Scalar>& operator-= (Scalar s);
  // Assign and mult operator
  inline const ntlVector3Dim<Scalar>& operator*= (const ntlVector3Dim<Scalar>& v);
  // Assign and mult operator
  inline const ntlVector3Dim<Scalar>& operator*= (Scalar s);
  // Assign and div operator
  inline const ntlVector3Dim<Scalar>& operator/= (const ntlVector3Dim<Scalar>& v);
  // Assign and div operator
  inline const ntlVector3Dim<Scalar>& operator/= (Scalar s);


  // output operator
  friend ostream& operator<< <>( ostream& os, const ntlVector3Dim<Scalar>& inst );
  // input operator
  friend istream& operator>> <>( istream& is, ntlVector3Dim<Scalar>& inst );
  
  // unary operator
  inline ntlVector3Dim<Scalar> operator- () const;

  // binary operator add
  inline ntlVector3Dim<Scalar> operator+ (const ntlVector3Dim<Scalar>&) const;
  // binary operator add
  inline ntlVector3Dim<Scalar> operator+ (Scalar) const;
  // binary operator sub
  inline ntlVector3Dim<Scalar> operator- (const ntlVector3Dim<Scalar>&) const;
  // binary operator sub
  inline ntlVector3Dim<Scalar> operator- (Scalar) const;
  // binary operator mult
  inline ntlVector3Dim<Scalar> operator* (const ntlVector3Dim<Scalar>&) const;
  // binary operator mult
  inline ntlVector3Dim<Scalar> operator* (Scalar) const;
  // binary operator div
  inline ntlVector3Dim<Scalar> operator/ (const ntlVector3Dim<Scalar>&) const;
  // binary operator div
  inline ntlVector3Dim<Scalar> operator/ (Scalar) const;

  // equality test operator
  inline bool      operator== (const ntlVector3Dim<Scalar>&) const;
  // inequality test operator

  inline bool      operator!= (const ntlVector3Dim<Scalar>&) const;

  // scalar product
  inline Scalar operator|( const ntlVector3Dim<Scalar> &v ) const;

  // norm
  inline Scalar getNorm() const;
  // norm without squareroot
  inline Scalar getNormNoSqrt() const;
  // normalize
  inline ntlVector3Dim<Scalar> getNormalized() const;
  // normalize and norm
  inline Scalar normalize();

  // vector product
  inline ntlVector3Dim<Scalar> operator^( const ntlVector3Dim<Scalar> &v ) const;

  // Projection normal to a vector
  inline ntlVector3Dim<Scalar>    getOrthogonalntlVector3Dim() const;
  // Project into a plane
  inline const ntlVector3Dim<Scalar>& projectNormalTo(const ntlVector3Dim<Scalar> &v);
  // Reflect at normal
  inline ntlVector3Dim<Scalar>    getReflectedAt(const ntlVector3Dim<Scalar>& n) const;
  // Refract vector at normal with two refraction indices
  inline ntlVector3Dim<Scalar>    refractAt(const ntlVector3Dim<Scalar>& normal, double nt, double nair, int &refRefl) const;
  
  // minimize
  inline const ntlVector3Dim<Scalar> &minimize(const ntlVector3Dim<Scalar> &);
  // maximize
  inline const ntlVector3Dim<Scalar> &maximize(const ntlVector3Dim<Scalar> &);

  // Calculate cross product of two vectors
  inline ntlVector3Dim<Scalar> crossProd(const ntlVector3Dim<Scalar> &) const;
  
  // access operator
  inline Scalar& operator[](unsigned int i);
  // access operator
  inline const Scalar& operator[](unsigned int i) const;

  // Get minimal vector length
  inline static Scalar getEpsilon( );

        //! Assume this vector is an RGB color, and convert it to HSV
        inline void rgbToHsv( void );

        //! Assume this vector is HSV and convert to RGB
        inline void hsvToRgb( void );

protected:
  
private:
  Scalar value[3];  //< Storage of vector values
};



//------------------------------------------------------------------------------
// TYPEDEFS
//------------------------------------------------------------------------------

// a 3D vector for graphics output, typically float?
//typedef ntlVector3Dim<float>  ntlVec3Gfx; 
typedef ntlVector3Dim<double>  ntlVec3Gfx; 

// a 3D vector with double precision
typedef ntlVector3Dim<double>  ntlVec3d; 

// a 3D vector with single precision
typedef ntlVector3Dim<float>   ntlVec3f; 

// a 3D integer vector
typedef ntlVector3Dim<int>     ntlVec3i; 

// Color uses double precision values
typedef ntlVector3Dim<double>  ntlColor;



//------------------------------------------------------------------------------
// STREAM FUNCTIONS
//------------------------------------------------------------------------------



/*************************************************************************
  Outputs the object in human readable form using the format
  [x,y,z]
  */
template<class Scalar>
ostream&
operator<<( ostream& os, const ntlVector3Dim<Scalar>& i )
{
  os << '[' << i.value[0] << ", " << i.value[1] << ", " << i.value[2] << ']';
  return os;
}



/*************************************************************************
  Reads the contents of the object from a stream using the same format
  as the output operator.
  */
template<class Scalar>
istream&
operator>>( istream& is, ntlVector3Dim<Scalar>& i )
{
  char c;
  char dummy[3];
  
  is >> c >> i.value[0] >> dummy >> i.value[1] >> dummy >> i.value[2] >> c;
  return is;
}


//------------------------------------------------------------------------------
// VECTOR inline FUNCTIONS
//------------------------------------------------------------------------------



/*************************************************************************
  Constructor.
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>::ntlVector3Dim( void )
{
#ifdef VECTOR_INIT_ZERO
  value[0] = value[1] = value[2] = 0;
#endif
}



/*************************************************************************
  Copy-Constructor.
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>::ntlVector3Dim( const ntlVector3Dim<Scalar> &v )
{
  value[0] = v.value[0];
  value[1] = v.value[1];
  value[2] = v.value[2];
}



/*************************************************************************
  Constructor for a vector from a single Scalar. All components of
  the vector get the same value.
  \param s The value to set
  \return The new vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>::ntlVector3Dim(Scalar s )
{
  value[0]= s;
  value[1]= s;
  value[2]= s;
}


/*************************************************************************
  Constructor for a vector from three Scalars.
  \param s1 The value for the first vector component
  \param s2 The value for the second vector component
  \param s3 The value for the third vector component
  \return The new vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>::ntlVector3Dim(Scalar s1, Scalar s2, Scalar s3)
{
  value[0]= s1;
  value[1]= s2;
  value[2]= s3;
}


/*************************************************************************
  Compute the vector product of two 3D vectors
  \param v Second vector to compute the product with
  \return A new vector with the product values
  */
template<class Scalar>
inline ntlVector3Dim<Scalar> 
ntlVector3Dim<Scalar>::operator^( const ntlVector3Dim<Scalar> &v ) const
{
  return ntlVector3Dim<Scalar>(value[1]*v.value[2] - value[2]*v.value[1],
                        value[2]*v.value[0] - value[0]*v.value[2],
                        value[0]*v.value[1] - value[1]*v.value[0]);
}


/*************************************************************************
  Copy a ntlVector3Dim componentwise.
  \param v vector with values to be copied
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator=( const ntlVector3Dim<Scalar> &v )
{
  value[0] = v.value[0];
  value[1] = v.value[1];
  value[2] = v.value[2];  
  return *this;
}


/*************************************************************************
  Copy a Scalar to each component.
  \param s The value to copy
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator=(Scalar s)
{
  value[0] = s;
  value[1] = s;
  value[2] = s;  
  return *this;
}


/*************************************************************************
  Add another ntlVector3Dim componentwise.
  \param v vector with values to be added
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator+=( const ntlVector3Dim<Scalar> &v )
{
  value[0] += v.value[0];
  value[1] += v.value[1];
  value[2] += v.value[2];  
  return *this;
}


/*************************************************************************
  Add a Scalar value to each component.
  \param s Value to add
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator+=(Scalar s)
{
  value[0] += s;
  value[1] += s;
  value[2] += s;  
  return *this;
}


/*************************************************************************
  Subtract another vector componentwise.
  \param v vector of values to subtract
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator-=( const ntlVector3Dim<Scalar> &v )
{
  value[0] -= v.value[0];
  value[1] -= v.value[1];
  value[2] -= v.value[2];  
  return *this;
}


/*************************************************************************
  Subtract a Scalar value from each component.
  \param s Value to subtract
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator-=(Scalar s)
{
  value[0]-= s;
  value[1]-= s;
  value[2]-= s;  
  return *this;
}


/*************************************************************************
  Multiply with another vector componentwise.
  \param v vector of values to multiply with
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator*=( const ntlVector3Dim<Scalar> &v )
{
  value[0] *= v.value[0];
  value[1] *= v.value[1];
  value[2] *= v.value[2];  
  return *this;
}


/*************************************************************************
  Multiply each component with a Scalar value.
  \param s Value to multiply with
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator*=(Scalar s)
{
  value[0] *= s;
  value[1] *= s;
  value[2] *= s;  
  return *this;
}


/*************************************************************************
  Divide by another ntlVector3Dim componentwise.
  \param v vector of values to divide by
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator/=( const ntlVector3Dim<Scalar> &v )
{
  value[0] /= v.value[0];
  value[1] /= v.value[1];
  value[2] /= v.value[2];  
  return *this;
}


/*************************************************************************
  Divide each component by a Scalar value.
  \param s Value to divide by
  \return Reference to self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::operator/=(Scalar s)
{
  value[0] /= s;
  value[1] /= s;
  value[2] /= s;
  return *this;
}


//------------------------------------------------------------------------------
// unary operators
//------------------------------------------------------------------------------


/*************************************************************************
  Build componentwise the negative this vector.
  \return The new (negative) vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator-() const
{
  return ntlVector3Dim<Scalar>(-value[0], -value[1], -value[2]);
}



//------------------------------------------------------------------------------
// binary operators
//------------------------------------------------------------------------------


/*************************************************************************
  Build a vector with another vector added componentwise.
  \param v The second vector to add
  \return The sum vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator+( const ntlVector3Dim<Scalar> &v ) const
{
  return ntlVector3Dim<Scalar>(value[0]+v.value[0],
                        value[1]+v.value[1],
                        value[2]+v.value[2]);
}


/*************************************************************************
  Build a vector with a Scalar value added to each component.
  \param s The Scalar value to add
  \return The sum vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator+(Scalar s) const
{
  return ntlVector3Dim<Scalar>(value[0]+s,
                        value[1]+s,
                        value[2]+s);
}


/*************************************************************************
  Build a vector with another vector subtracted componentwise.
  \param v The second vector to subtract
  \return The difference vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator-( const ntlVector3Dim<Scalar> &v ) const
{
  return ntlVector3Dim<Scalar>(value[0]-v.value[0],
                        value[1]-v.value[1],
                        value[2]-v.value[2]);
}


/*************************************************************************
  Build a vector with a Scalar value subtracted componentwise.
  \param s The Scalar value to subtract
  \return The difference vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator-(Scalar s ) const
{
  return ntlVector3Dim<Scalar>(value[0]-s,
                        value[1]-s,
                        value[2]-s);
}



/*************************************************************************
  Build a vector with another vector multiplied by componentwise.
  \param v The second vector to muliply with
  \return The product vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator*( const ntlVector3Dim<Scalar>& v) const
{
  return ntlVector3Dim<Scalar>(value[0]*v.value[0],
                        value[1]*v.value[1],
                        value[2]*v.value[2]);
}


/*************************************************************************
  Build a ntlVector3Dim with a Scalar value multiplied to each component.
  \param s The Scalar value to multiply with
  \return The product vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator*(Scalar s) const
{
  return ntlVector3Dim<Scalar>(value[0]*s, value[1]*s, value[2]*s);
}


/*************************************************************************
  Build a vector divided componentwise by another vector.
  \param v The second vector to divide by
  \return The ratio vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator/(const ntlVector3Dim<Scalar>& v) const
{
  return ntlVector3Dim<Scalar>(value[0]/v.value[0],
                        value[1]/v.value[1],
                        value[2]/v.value[2]);
}



/*************************************************************************
  Build a vector divided componentwise by a Scalar value.
  \param s The Scalar value to divide by
  \return The ratio vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::operator/(Scalar s) const
{
  return ntlVector3Dim<Scalar>(value[0]/s,
                        value[1]/s,
                        value[2]/s);
}



/*************************************************************************
  Test two ntlVector3Dims for equality based on the equality of their
  values within a small threshold.
  \param c The second vector to compare
  \return true if both are equal
  \sa getEpsilon()
  */
template<class Scalar>
inline bool
ntlVector3Dim<Scalar>::operator== (const ntlVector3Dim<Scalar>& c) const
{
  return (ABS(value[0]-c.value[0]) + 
          ABS(value[1]-c.value[1]) + 
          ABS(value[2]-c.value[2]) < VECTOR_EPSILON);
}


/*************************************************************************
  Test two ntlVector3Dims for inequality based on the inequality of their
  values within a small threshold.
  \param c The second vector to compare
  \return false if both are equal
  \sa getEpsilon()
  */
template<class Scalar>
inline bool
ntlVector3Dim<Scalar>::operator!= (const ntlVector3Dim<Scalar>& c) const
{
  return (!(ABS(value[0]-c.value[0]) + 
            ABS(value[1]-c.value[1]) + 
            ABS(value[2]-c.value[2]) < VECTOR_EPSILON));
}





/*************************************************************************
  Get a particular component of the vector.
  \param i Number of Scalar to get
  \return Reference to the component
  */
template<class Scalar>
inline Scalar&
ntlVector3Dim<Scalar>::operator[]( unsigned int i )
{
  return value[i];
}


/*************************************************************************
  Get a particular component of a constant vector.
  \param i Number of Scalar to get
  \return Reference to the component
  */
template<class Scalar>
inline const Scalar&
ntlVector3Dim<Scalar>::operator[]( unsigned int i ) const
{
  return value[i];
}



//------------------------------------------------------------------------------
// scalar functions
//------------------------------------------------------------------------------



/*************************************************************************
  Compute the scalar product with another vector.
  \param v The second vector to work with
  \return The value of the scalar product
  */
template<class Scalar>
inline Scalar
ntlVector3Dim<Scalar>::operator|(const ntlVector3Dim<Scalar> &v ) const
{
  return value[0]*v.value[0] + value[1]*v.value[1] + value[2]*v.value[2];
}


//------------------------------------------------------------------------------
// norm/alizing stuff
//------------------------------------------------------------------------------


/*************************************************************************
  Compute the length (norm) of the vector.
  \return The value of the norm
  */
template<class Scalar>
inline Scalar
ntlVector3Dim<Scalar>::getNorm() const
{
  Scalar l = value[0]*value[0] + value[1]*value[1] + value[2]*value[2];
  return (fabs(l-1.) < VECTOR_EPSILON*VECTOR_EPSILON) ? 1. : sqrt(l);
}


/*************************************************************************
  Same as getNorm but doesnt sqrt  
  */
template<class Scalar>
inline Scalar
ntlVector3Dim<Scalar>::getNormNoSqrt() const
{
  return value[0]*value[0] + value[1]*value[1] + value[2]*value[2];
}


/*************************************************************************
  Compute a normalized vector based on this vector.
  \return The new normalized vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::getNormalized() const
{
  Scalar l = value[0]*value[0] + value[1]*value[1] + value[2]*value[2];
  if (fabs(l-1.) < VECTOR_EPSILON*VECTOR_EPSILON)
    return *this; /* normalized "enough"... */
  else if (l > VECTOR_EPSILON*VECTOR_EPSILON)
  {
    double fac = 1./sqrt(l);
    return ntlVector3Dim<Scalar>(value[0]*fac, value[1]*fac, value[2]*fac);
  }
  else
    return ntlVector3Dim<Scalar>((Scalar)0);
}


/*************************************************************************
  Compute the norm of the vector and normalize it.
  \return The value of the norm
  */
template<class Scalar>
inline Scalar
ntlVector3Dim<Scalar>::normalize()
{
  double norm;
  Scalar l = value[0]*value[0] + value[1]*value[1] + value[2]*value[2];  
  if (fabs(l-1.) < VECTOR_EPSILON*VECTOR_EPSILON) {
    norm = 1.;
        } else if (l > VECTOR_EPSILON*VECTOR_EPSILON) {
    norm = sqrt(l);
    double fac = 1./norm;
    value[0] *= fac;
    value[1] *= fac;
    value[2] *= fac; 
        } else {
    value[0]= value[1]= value[2]= 0;
    norm = 0.;
  }
  return (Scalar)norm;

        // OLD
  if (l < VECTOR_EPSILON*VECTOR_EPSILON)
  {
    value[0]= value[1]= value[2]= 0;
    norm = 0.;
  }
  else if (fabs(l-1.) > VECTOR_EPSILON*VECTOR_EPSILON)
  {
    norm = sqrt(l);
    double fac = 1./norm;
    value[0] *= fac;
    value[1] *= fac;
    value[2] *= fac;
  }
  else
    norm = 1.;
  
  return (Scalar)norm;
}


/*************************************************************************
  Compute a vector that is orthonormal to self. Nothing else can be assumed
  for the direction of the new vector.
  \return The orthonormal vector
  */
template<class Scalar>
ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::getOrthogonalntlVector3Dim() const
{
  // Determine the  component with max. absolute value
  int max= (fabs(value[0]) > fabs(value[1])) ? 0 : 1;
  max= (fabs(value[max]) > fabs(value[2])) ? max : 2;

  /*************************************************************************
    Choose another axis than the one with max. component and project
    orthogonal to self
    */
  ntlVector3Dim<Scalar> vec(0.0);
  vec[(max+1)%3]= 1;
  vec.normalize();
  vec.projectNormalTo(this->getNormalized());
  return vec;
}


/*************************************************************************
  Projects the vector into a plane normal to the given vector, which must
  have unit length. Self is modified.
  \param v The plane normal
  \return The projected vector
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar>&
ntlVector3Dim<Scalar>::projectNormalTo(const ntlVector3Dim<Scalar> &v)
{
  Scalar sprod = (*this|v);
  value[0]= value[0] - v.value[0] * sprod;
  value[1]= value[1] - v.value[1] * sprod;
  value[2]= value[2] - v.value[2] * sprod;  
  return *this;
}



//------------------------------------------------------------------------------
// Other helper functions
//------------------------------------------------------------------------------



/*************************************************************************
  Compute a vector, that is self (as an incoming
  vector) reflected at a surface with a distinct normal vector. Note
  that the normal is reversed, if the scalar product with it is positive.
  \param n The surface normal
  \return The new reflected vector
  */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::getReflectedAt(const ntlVector3Dim<Scalar>& n) const
{
  ntlVector3Dim<Scalar> nn= ((*this | n) > 0) ? -n : n;
  return *this - nn * (2 * (nn|*this));
  //return ntlVector3Dim<Scalar>(0,-1,0);
}



/*************************************************************************
 * My own refraction calculation
 * Taken from Glassner's book, section 5.2 (Heckberts method)
 */
template<class Scalar>
inline ntlVector3Dim<Scalar>
ntlVector3Dim<Scalar>::refractAt(const ntlVector3Dim<Scalar>& normal, double nt, double nair, int &refRefl) const
{
        //double nair = mpGlob->getRefIndex();
        //double nt = myRefIndex;
        double eta = nair / nt;
        double n = -((*this) | normal);
        double t = 1.0 + eta*eta* (n*n-1.0);
        if(t<0.0) {
                // we have total reflection!
                refRefl = 1;
        } else {
                // normal reflection
                t = eta*n - sqrt(t);
                return( (*this)*eta + normal*t );
        }
        return (*this);
}


/*************************************************************************
  Calculate the cross product of this and another vector
 */
template<class Scalar>
inline ntlVector3Dim<Scalar> 
ntlVector3Dim<Scalar>::crossProd(const ntlVector3Dim<Scalar> &v) const
{
  ntlVector3Dim cp( (value[1]*v[2] - value[2]*v[1]),
                    (value[2]*v[0] - value[0]*v[2]),
                    (value[0]*v[1] - value[1]*v[0]) );
  return cp;
}



/*************************************************************************
  Minimize the vector, i.e. set each entry of the vector to the minimum
  of both values.
  \param pnt The second vector to compare with
  \return Reference to the modified self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar> &
ntlVector3Dim<Scalar>::minimize(const ntlVector3Dim<Scalar> &pnt)
{
  for (unsigned int i = 0; i < 3; i++)
    value[i] = MIN(value[i],pnt[i]);
  return *this;
}



/*************************************************************************
  Maximize the vector, i.e. set each entry of the vector to the maximum
  of both values.
  \param pnt The second vector to compare with
  \return Reference to the modified self
  */
template<class Scalar>
inline const ntlVector3Dim<Scalar> &
ntlVector3Dim<Scalar>::maximize(const ntlVector3Dim<Scalar> &pnt)
{
  for (unsigned int i = 0; i < 3; i++)
    value[i] = MAX(value[i],pnt[i]);
  return *this;
}



/*************************************************************************
  Get minimal vector length value that can be discriminated.
  \return The minimal value
  \sa VECTOR_EPSILON
  */
template<class Scalar>
inline Scalar
ntlVector3Dim<Scalar>::getEpsilon()
{
  return VECTOR_EPSILON;
}


/*************************************************************************
 * Assume this vector is an RGB color, and convert it to HSV
 */
template<class Scalar>
inline void 
ntlVector3Dim<Scalar>::rgbToHsv( void )
{
        Scalar h=0,s=0,v=0;
        Scalar maxrgb, minrgb, delta;
        // convert to hsv...
        maxrgb = value[0];
        int maxindex = 1;
        if(value[2] > maxrgb){ maxrgb = value[2]; maxindex = 2; }
        if(value[1] > maxrgb){ maxrgb = value[1]; maxindex = 3; }
        minrgb = value[0];
        if(value[2] < minrgb) minrgb = value[2];
        if(value[1] < minrgb) minrgb = value[1];

        v = maxrgb;
        delta = maxrgb-minrgb;

        if(maxrgb > 0) s = delta/maxrgb;
        else s = 0;

        h = 0;
        if(s > 0) {
                if(maxindex == 1) {
                        h = ((value[1]-value[2])/delta)  + 0.0; }
                if(maxindex == 2) {
                        h = ((value[2]-value[0])/delta)  + 2.0; }
                if(maxindex == 3) {
                        h = ((value[0]-value[1])/delta)  + 4.0; }
                h *= 60.0;
                if(h < 0.0) h += 360.0;
        }

        value[0] = h;
        value[1] = s;
        value[2] = v;
}

/*************************************************************************
 * Assume this vector is HSV and convert to RGB
 */
template<class Scalar>
inline void 
ntlVector3Dim<Scalar>::hsvToRgb( void )
{
        Scalar h = value[0], s = value[1], v = value[2];
        Scalar r=0,g=0,b=0;
        Scalar p,q,t, fracth;
        int floorh;
        // ...and back to rgb
        if(s == 0) {
                r = g = b = v; }
        else {
                h /= 60.0;
                floorh = (int)h;
                fracth = h - floorh;
                p = v * (1.0 - s);
                q = v * (1.0 - (s * fracth));
                t = v * (1.0 - (s * (1.0 - fracth)));
                switch (floorh) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
                }
        }

        value[0] = r;
        value[1] = g;
        value[2] = b;
}


#endif /* NTL_VECTOR3DIM_HH */
