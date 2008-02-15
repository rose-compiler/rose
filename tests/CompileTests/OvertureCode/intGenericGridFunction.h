#ifndef INT_GENERIC_GRID_FUNCTION_H
#define INT_GENERIC_GRID_FUNCTION_H "intGenericGridFunction.h"

#include "ReferenceCounting.h"
#include "OvertureTypes.h"

class GenericGrid;  // forward declaration

//---------------------------------------------------------------------------
//  This is a generic grid function from which other types of grid functions
//  can be derived. 
//---------------------------------------------------------------------------
class intGenericGridFunction : public ReferenceCounting
{
 public:
  GenericGrid *grid;

  intGenericGridFunction(){ grid=NULL; }
  intGenericGridFunction( const intGenericGridFunction & ,
                             const CopyType=DEEP ) { }
  ~intGenericGridFunction (){}

  intGenericGridFunction ( GenericGrid *grid0 ){ grid=grid0; }

  intGenericGridFunction & operator= ( const intGenericGridFunction & X )
    { grid = X.grid; return *this; }
  void reference( const intGenericGridFunction & ){}
  virtual void breakReference(){}
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return intGenericGridFunction::operator=( (intGenericGridFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (intGenericGridFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new intGenericGridFunction(*this, ct); }
  
};
#endif // intGenericGridFunction.h
