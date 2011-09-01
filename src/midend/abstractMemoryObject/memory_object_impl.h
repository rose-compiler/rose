#ifndef memory_object_impl_INCLUDED
#define memory_object_impl_INCLUDED

#include "memory_object.h"

namespace AbstractMemoryObject 
{

  // Big picture of this implementation
  // How the multiple inheritance results in the classes here
  //
  //                      Scalar            LabeledAggregate        Array       Pointer
  // -----------------------------------------------------------------------------
  // Expression Objects | ScalarExp          LabAggreExp 
  // Named objects      | ScalarNamedObj    
  // Aliased Objects    | ScalarAliasedObj  LabeledAggregateAliasedObj ArrayAliasedObj  PointerAliasedObj

  // A simple implementation of the abstract memory object interface
  // Four categories: scalar, labeled aggregate, array, and pointer
  class Scalar_Impl : public Scalar 
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
  };

  class LabeledAggregate_Impl : public LabeledAggregate
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    

  };

  class NamedObj; 
  class LabeledAggregateField_Impl : public LabeledAggregateField
  {
    public:
      LabeledAggregateField_Impl ( ObjSet* f, LabeledAggregate* p): field (f), parent(p) {}
      std::string getName(); // field name
      size_t getIndex(); // The field's index within its parent object. The first field has index 0.

      ObjSet* getField() { return field;}; // Pointer to an abstract description of the field
      void setField(ObjSet* f) {field = f;}; // Pointer to an abstract description of the field

      LabeledAggregate* getParent() {return parent;};
      void setParent(LabeledAggregate* p) {parent = p; };

      std::string toString();

    private:
      ObjSet* field; // this should be a named obj 
      LabeledAggregate* parent; 
  };


  class Array_Impl : public Array
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
  };

  class Pointer_Impl: public Pointer 
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
  };

  // The connection to the ROSE AST, all concrete type, size , etc. information come from this side
  // -----------------------------------------------------------------------------------------------
  //
  // Three kinds of memory objects in ROSE AST: each of them can be one of the four categories above.
  // 1) SgExpression temporary variables: each SgExpression which is not named memory objects 
  // 2) named memory objects : one object for each named variable 
  // 3) aliased memory objects: one object for each type

  class ExprObj // one object for each SgExpression which does not have a corresponding symbol
  { 
    public:
      SgExpression* anchor_exp; 
      SgType* type; 
      ObjSet* parent; // Can we decide that the parent mem object is always ExprObj?? 
                       // Does a parent ever exist for ExprObj???     
      ExprObj (SgExpression* a, SgType* t, ObjSet* p): anchor_exp(a), type(t),parent(p){};
      SgType* getType() {return type;}
  };

 // Correspond to variables that are explicit named in the source code
 // Including: local, global, and static variables, as well as their fields
 // Named objects may not directly alias each other since they must be stored disjointly
 // Two named objects are equal to each other if they correspond to the same entry in the application's symbol table. 
 // (?? Should an implementation enforce unique named objects for a unique symbol entry??)
  class NamedObj  // one object for each named variable with a symbol
  { 
    public:
      SgSymbol* anchor_symbol; 
      SgType* type; 
      ObjSet* parent;  //Only exists for compound variables like a.b, where a is b's parent

      //Is this always true that the parent of a named object must be an expr object?
      NamedObj (SgSymbol* a, SgType* t, ObjSet* p):anchor_symbol(a), type(t),parent(p){};
      SgType* getType() {return type;}
      ObjSet* getParent() {return parent; } 
      SgSymbol* getSymbol() {return anchor_symbol;}

      std::string getName() {return anchor_symbol->get_name().getString(); }

      std::string toString(); 

      bool operator == (ObjSet& o2) ;
      bool operator == (NamedObj& o2) ;
      bool operator < ( NamedObj& o2);
  };

  //  memory regions that may be accessible via a pointer, such as heap memory
  //  This implementation does not track accurate aliases, an aliased memory object and 
  //  an aliased or named object may be equal if they have the same type
  class AliasedObj 
  {  // One object for each type
    public: 
      SgType* type; 
      AliasedObj (SgType* t): type(t) {};
      SgType* getType() {return type;}
      std::string toString(); 

      bool operator == (ObjSet& o2) ;
      bool operator == (AliasedObj & o2) ;
      bool operator < ( AliasedObj& o2);
  };


  //Derived classes for each kind of each category
  // expression object ------------------------------
  class ScalarExprObj: public Scalar_Impl, public ExprObj
  {
  };

  class LabeledAggregateExprObj: public LabeledAggregate_Impl, public ExprObj
  {
  };

  class ArrayExprObj: public Array_Impl, public ExprObj
  {};

  class PointerExprbj: public Pointer_Impl, public ExprObj
  {};

  // named object --------------------------------
  //
   class ScalarNamedObj: public Scalar_Impl, public NamedObj 
  {
    public:
      ScalarNamedObj (SgSymbol* s, SgType* t, ObjSet* p): NamedObj (s,t,p) {}
      std::set<SgType*> getType();
      bool operator == (ObjSet& o2) ;
      std::string toString();
  };

  class LabeledAggregateNamedObj: public LabeledAggregate_Impl, public NamedObj
  {
    public:
      LabeledAggregateNamedObj (SgSymbol* s, SgType* t, ObjSet* p);
      std::set<SgType*> getType();
      size_t fieldCount() {return elements.size(); };
      // Returns a list of field
      std::vector<LabeledAggregateField*> getElements() const {return elements;};

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
     // TODO bool operator == (const LabeledAggregate& that) const;
      //Total order relations (implemented by interface)
      //bool operator < (const LabeledAggregate& that) const;

      std::string toString();
    private:
      std::vector<LabeledAggregateField*> elements; 
  };

  class ArrayNamedObj: public Array_Impl, public NamedObj
  {};

  class PointerNamedObj: public Pointer_Impl, public NamedObj
  {
    public:
      PointerNamedObj   (SgSymbol* s, SgType* t, ObjSet* p): NamedObj (s,t,p) {}
      std::set<SgType*> getType();
     // used for a pointer to non-array
      ObjSet* getDereference () ;
      // used for a pointer to an array
      ObjSet * getElements() ;
     // Returns true if this pointer refers to the same abstract object as that pointer.
      bool equalPoints(Pointer & that);
     // Returns true if this object and that object may/must refer to the same pointer memory object.
      bool operator == (ObjSet& o2) ;
      std::string toString();
  };
 
  // aliased object -----------------------------
   class ScalarAliasedObj: public Scalar_Impl, public AliasedObj
  {
    public:
      ScalarAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      bool operator == (ObjSet& o2) ; 
      std::string toString();

  };

  class LabeledAggregateAliasedObj : public  LabeledAggregate_Impl, public AliasedObj
  {
    public:
      LabeledAggregateAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      //TODO
      // size_t fieldCount();
      // std::list<LabeledAggregateField*> getElements() const;
      bool operator == (ObjSet& o2);
      std::string toString();
  };

  class ArrayAliasedObj: public Array_Impl, public AliasedObj
  {
    public:
      ArrayAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      
      //TODO
      // ObjSet* getElements();
      //  ObjSet* getElements(AbstractIndex* ai);
      //  getNumDims();

      bool operator == (ObjSet& o2);
      std::string toString();
  };

  class PointerAliasedObj: public Pointer_Impl, public AliasedObj
  {
    public:
      PointerAliasedObj (SgType* t): AliasedObj(t){}
      // TODO ObjSet* getDereference () const;
      // ObjSet * getElements() const;
      // bool equalPoints(const Pointer & that);
      std::set<SgType*> getType();
      bool operator == (ObjSet& o2);
      std::string toString();
  };

  // builder for different objects -----------------------
  // Create an aliased obj set from a type. It can return NULL since not all types are supported.
  ObjSet* createAliasedObjSet(SgType*t);  // One object per type, Type based alias analysis

  ObjSet* createNamedObjSet(SgSymbol* anchor_symbol, SgType* t, ObjSet* parent); // any 
  ObjSet* createObjSet(SgVarRefExp* r); // create NamedObjSet or AliasedObjSet (for pointer type) from a variable reference 
  ObjSet* createExpressionObjSet(SgExpression* anchor_exp, SgType*t, ObjSet* parent); 
  
  // Helper functions for debugging
  void dump_aliased_objset_map (); 

  // A helper function to decide if two types are aliased
  // two cases: 1 they are the same type
  //            2 they have overlap (one type is a subtype of the other)
  bool isAliased (SgType * t1, SgType * t2 ); 

  // a helper function to check if a symbol is corresponding to a member variable declaration within SgClassDefinition or not
  bool isMemberVariableDeclarationSymbol(SgSymbol * s);


} // end namespace

#endif
