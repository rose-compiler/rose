#ifndef memory_object_impl_INCLUDED
#define memory_object_impl_INCLUDED

#include "memory_object.h"

namespace AbstractMemoryObject 
{

  // A simple implementation of the abstract memory object interface
  // Four categories: scalar, labeled aggregate, array, and pointer
  class scalar_Impl : public scalar 
  {

  };

  class labeledAggregate_Impl : public labeledAggregate
  {

  };

  class array_Impl : public array
  {
  };

  class pointer_Impl: public pointer 
  {

  };

  // Three kinds of memory objects in ROSE AST: each of them can be one of the four categories above.
  // 1) SgExpression temporary variables: each SgExpression which is not named memory objects 
  // 2) named memory objects : one object for each named variable 
  // 3) aliased memory objects: one object for each type

  class ExprObj 
  { 
    public:
      SgExpression* anchor; 
      SgType* type; 
      ObjSet* parent; // Can we decide that the parent mem object is always ExprObj?? 
      ExprObj (SgExpression* a, SgType* t, ObjSet* p): anchor(a), type(t),parent(p){};
  };

  class NamedObj 
  { 
    public:
      SgSymbol* anchor; 
      SgType* type; 
      ObjSet* parent;  //Is this always true that the parent of a named object must be an expr object?
      NamedObj (SgSymbol* a, SgType* t, ObjSet* p):anchor(a), type(t),parent(p){};
  };

  class AliasedObj { 
    public: 
    SgType* type; 
    AliasedObj (SgType* t): type(t) {};
  };

  //Derived classes for each kind of each category
  // expression object
  class scalarExprObj: public scalar_Impl, public ExprObj
  {
  };

  class labeledAggregateExprObj: public labeledAggregate_Impl, public ExprObj
  {
  };

  class arrayExprObj: public array_Impl, public ExprObj
  {};

  class pointerExprbj: public pointer_Impl, public ExprObj
  {};

  // named object
   class scalarNamedObj: public scalar_Impl, public NamedObj 
  {
  };

  class labeledAggregateNamedObj: public labeledAggregate_Impl, public NamedObj
  {
  };

  class arrayNamedObj: public array_Impl, public NamedObj
  {};

  class pointerNamedObj: public pointer_Impl, public NamedObj
  {};

 
  // aliased object
   class scalarAliasedObj: public scalar_Impl, public AliasedObj
  {
  };

  class labeledAggregateAliasedObj:public  labeledAggregate_Impl, public AliasedObj
  {
  };

  class arrayAliasedObj: public array_Impl, public AliasedObj
  {};

  class pointerAliasedObj: public pointer_Impl, public AliasedObj
  {};


} // end namespace

#endif
