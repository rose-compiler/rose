#ifndef MATRIX_MAPPING_H
#define MATRIX_MAPPING_H "matrixMapping.h"

#include <A++.h>
#include <string.h>
#include <assert.h>
#include "Mapping.h"

class MatrixMapping : public Mapping{
//-------------------------------------------------------------
//
// Matrix Mapping: transformation represented by a 3x4 matrix
// can be used for scaling, rotation, shifts etc.
//     x(0:2) <- Matrix(0:2,0:2)*t(0:2) + matrix(0:2,3)
//
//------------------------------------------------------------
protected:
  aString className;

public:

  RealArray matrix;
  RealArray matrixInverse;
  
  MatrixMapping( int domainDimension=3, int rangeDimension=3 );

  // Copy constructor is deep by default
  MatrixMapping( const MatrixMapping &, const CopyType copyType=DEEP );

  ~MatrixMapping();

  MatrixMapping & operator =( const MatrixMapping & X );

  void rotate( const int axis, const real theta );
  void rotate( const RealArray & rotate ); // specify a 3x3 rotation matrix

  void scale( const real scalex=1., const real scaley=1., const real scalez=1. );

  void shift( const real shiftx=1., const real shifty=1., const real shiftz=1. );

  // reset the matrix to the identity:
  void reset();

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return  MatrixMapping::className; }

  void map( const realArray & r, realArray & x, realArray & xr = Overture::nullRealDistributedArray(),
            MappingParameters & params =Overture::nullMappingParameters() );

  void inverseMap( const realArray & x, realArray & r,realArray & rx = Overture::nullRealDistributedArray(),
                   MappingParameters & params =Overture::nullMappingParameters() );

  void basicInverse( const realArray & x, realArray & r, realArray & rx = Overture::nullRealDistributedArray(),
                   MappingParameters & params =Overture::nullMappingParameters() );

  // utility routines
  static void matrixMatrixProduct( RealArray & m1, const RealArray & m2, const RealArray & m3 );
  static void matrixVectorProduct( RealArray & v1, const RealArray & m2, const RealArray & v3 );
  static int matrixInversion( RealArray & m1Inverse, const RealArray & m1 );

  private:

  //
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((MatrixMapping &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((MatrixMapping &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new MatrixMapping(*this, ct); }
};

#endif 

