#ifndef FLOAT_GENERIC_GRID_FUNCTION_H
#define FLOAT_GENERIC_GRID_FUNCTION_H "floatGenericGridFunction.h"

#include "ReferenceCounting.h"
#include "OvertureTypes.h"

class GenericGrid;  // forward declaration

//---------------------------------------------------------------------------
//  This is a generic grid function from which other types of grid functions
//  can be derived. 
//---------------------------------------------------------------------------
class floatGenericGridFunction : public ReferenceCounting
{
 public:
  GenericGrid *grid;

  floatGenericGridFunction(){ grid=NULL; }
  floatGenericGridFunction( const floatGenericGridFunction & ,
                             const CopyType=DEEP ) { }
  ~floatGenericGridFunction (){}

  floatGenericGridFunction ( GenericGrid *grid0 ){ grid=grid0; }

  floatGenericGridFunction & operator= ( const floatGenericGridFunction & X )
    { grid = X.grid; return *this; }
  void reference( const floatGenericGridFunction & ){}
  virtual void breakReference(){}
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return floatGenericGridFunction::operator=( (floatGenericGridFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (floatGenericGridFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new floatGenericGridFunction(*this, ct); }
  
};
#endif // floatGenericGridFunction.h
