#ifndef MATRIX_TRANSFORM
#define MATRIX_TRANSFORM

#include "ComposeMapping.h"
#include "MatrixMapping.h"

class MatrixTransform : public ComposeMapping{
//----------------------------------------------------------------
//  Rotate, Scale and or Shift a Mapping
//  ------------------------------------
//
//
// Constructor example:
//   ComposeMapping mapc( mapa,mapb );
// ...means
//     mapc <-  (mapb o mapa)
//  mapc means to apply mapa followed by mapb
//----------------------------------------------------------------
private:
  aString className;

 public:
  MatrixMapping *matrix;        // this mapping is use to rotate/scale/shift


  MatrixTransform( );

  // Apply a matrix transform to a given mapping
  MatrixTransform(Mapping & map);


  // Copy constructor is deep by default
  MatrixTransform( const MatrixTransform &, const CopyType copyType=DEEP );

  ~MatrixTransform();

  MatrixTransform & operator =( const MatrixTransform & X );

  // reset the matrix to the identity:
  void reset();

  void rotate( const int axis, const real theta );
  void rotate( const RealArray & rotate ); // specify a 3x3 rotation matrix

  void scale( const real scalex=1., const real scaley=1., const real scalez=1. );

  void shift( const real shiftx=0., const real shifty=0., const real shiftz=0. );

  void display( const aString & label=blankString) const;

  virtual int get( const GenericDataBase & dir, const aString & name);    // get from a database file
  virtual int put( GenericDataBase & dir, const aString & name) const;    // put to a database file

  Mapping* make( const aString & mappingClassName );

  aString getClassName() const { return MatrixTransform::className; }

  int update( MappingInformation & mapInfo ) ;


  private:

  int setMappingProperties();
  
  //  Virtual member functions used only through class ReferenceCounting:
  //
    virtual ReferenceCounting& operator=(const ReferenceCounting& x)
      { return operator=((MatrixTransform &)x); }
    virtual void reference( const ReferenceCounting& x) 
      { reference((MatrixTransform &)x); }     // *** Conversion to this class for the virtual = ****
    virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
      { return ::new MatrixTransform(*this, ct); }
  };


#endif
