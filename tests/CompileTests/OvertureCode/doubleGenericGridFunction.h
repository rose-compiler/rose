#ifndef DOUBLE_GENERIC_GRID_FUNCTION_H
#define DOUBLE_GENERIC_GRID_FUNCTION_H "doubleGenericGridFunction.h"

#include "ReferenceCounting.h"
#include "OvertureTypes.h"

class GenericGrid;  // forward declaration

//---------------------------------------------------------------------------
//  This is a generic grid function from which other types of grid functions
//  can be derived. 
//---------------------------------------------------------------------------
class doubleGenericGridFunction : public ReferenceCounting
{
 public:
  GenericGrid *grid;

  doubleGenericGridFunction(){ grid=NULL; }
  doubleGenericGridFunction( const doubleGenericGridFunction & ,
                             const CopyType=DEEP ) { }
  ~doubleGenericGridFunction (){}

  doubleGenericGridFunction ( GenericGrid *grid0 ){ grid=grid0; }

  doubleGenericGridFunction & operator= ( const doubleGenericGridFunction & X )
    { grid = X.grid; return *this; }
  void reference( const doubleGenericGridFunction & ){}
  virtual void breakReference(){}
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return doubleGenericGridFunction::operator=( (doubleGenericGridFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (doubleGenericGridFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new doubleGenericGridFunction(*this, ct); }
  
};
#endif // doubleGenericGridFunction.h
