#ifndef DOUBLEGENERIC_DOUBLE_GRID_COLLECTION_FUNCTION
#define DOUBLEGENERIC_DOUBLE_GRID_COLLECTION_FUNCTION "doubleGenericGridCollectionFunction.h"

#include "doubleGenericGridFunction.h" 
#include "ReferenceCounting.h"
#include "ListOfDoubleGenericGridFunction.h"

class GenericGridCollection;  // forward declaration

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class doubleGenericGridCollectionFunction : public ReferenceCounting
{
 public:
  
  ListOfDoubleGenericGridFunction genericGridFunctionList;
  GenericGridCollection *gridList;

  doubleGenericGridCollectionFunction(){}
  doubleGenericGridCollectionFunction(const doubleGenericGridCollectionFunction& ,
                                      const CopyType =DEEP){}
  virtual ~doubleGenericGridCollectionFunction(){}
  doubleGenericGridCollectionFunction( GenericGridCollection *gridList );
  doubleGenericGridCollectionFunction & operator= ( const doubleGenericGridCollectionFunction & X );
  doubleGenericGridFunction & operator[]( const int grid );
  void reference( const doubleGenericGridCollectionFunction & ){};
  virtual void breakReference(){};
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return operator=( (doubleGenericGridCollectionFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (doubleGenericGridCollectionFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new doubleGenericGridCollectionFunction(*this, ct); }
};  



#endif 
