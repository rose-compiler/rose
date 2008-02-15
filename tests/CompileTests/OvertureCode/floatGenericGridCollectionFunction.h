#ifndef FLOATGENERIC_FLOAT_GRID_COLLECTION_FUNCTION
#define FLOATGENERIC_FLOAT_GRID_COLLECTION_FUNCTION "floatGenericGridCollectionFunction.h"

#include "floatGenericGridFunction.h" 
#include "ReferenceCounting.h"
#include "ListOfFloatGenericGridFunction.h"

class GenericGridCollection;  // forward declaration

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
class floatGenericGridCollectionFunction : public ReferenceCounting
{
 public:
  
  ListOfFloatGenericGridFunction genericGridFunctionList;
  GenericGridCollection *gridList;

  floatGenericGridCollectionFunction(){}
  floatGenericGridCollectionFunction(const floatGenericGridCollectionFunction& ,
                                      const CopyType =DEEP){}
  virtual ~floatGenericGridCollectionFunction(){}
  floatGenericGridCollectionFunction( GenericGridCollection *gridList );
  floatGenericGridCollectionFunction & operator= ( const floatGenericGridCollectionFunction & X );
  floatGenericGridFunction & operator[]( const int grid );
  void reference( const floatGenericGridCollectionFunction & ){};
  virtual void breakReference(){};
 private:
  virtual ReferenceCounting& operator=( const ReferenceCounting & x)
    { return operator=( (floatGenericGridCollectionFunction &) x ); }
  virtual void reference( const ReferenceCounting & x)
    { reference( (floatGenericGridCollectionFunction &) x ); }
  virtual ReferenceCounting* virtualConstructor( const CopyType ct = DEEP ) const
    { return ::new floatGenericGridCollectionFunction(*this, ct); }
};  



#endif 
