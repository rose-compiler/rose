#ifndef INTGENERIC_INT_GRID_COLLECTION_FUNCTION
#define INTGENERIC_INT_GRID_COLLECTION_FUNCTION "intGenericGridCollectionFunction.h"

#include "intGenericGridFunction.h" 
#include "ReferenceCounting.h"
#include "ListOfIntGenericGridFunction.h"

class GenericGridCollection;  // forward declaration

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class intGenericGridCollectionFunction : public ReferenceCounting
{
 public:
  
  ListOfIntGenericGridFunction genericGridFunctionList;
  GenericGridCollection *gridList;

  intGenericGridCollectionFunction(){}
  intGenericGridCollectionFunction(const intGenericGridCollectionFunction& ,
                                      const CopyType =DEEP){}
  virtual ~intGenericGridCollectionFunction(){}
  intGenericGridCollectionFunction( GenericGridCollection *gridList );
  intGenericGridCollectionFunction & operator= ( const intGenericGridCollectionFunction & X );
  intGenericGridFunction & operator[]( const int grid );
  void reference( const intGenericGridCollectionFunction & ){};
  virtual void breakReference(){};
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return operator=( (intGenericGridCollectionFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (intGenericGridCollectionFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new intGenericGridCollectionFunction(*this, ct); }
};  



#endif 
