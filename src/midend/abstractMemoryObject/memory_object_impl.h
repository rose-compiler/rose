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

      size_t fieldCount() {return elements.size(); };
      // Returns a list of fields
      std::vector<LabeledAggregateField*> getElements() const {return elements;};
      std::vector<LabeledAggregateField*>& getElements() {return elements;};
    private:
      std::vector<LabeledAggregateField*> elements; 
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


  //! A set of index values, could have constant integer values or unknown values
  class IndexSet
  {
    public:
      enum Index_type {
        Integer_type = 0,
        Unknown_type 
      };

      Index_type getType() {return type; };

      size_t getSize() {return 1;}; // Simple thing first

      IndexSet (Index_type t):type(t){} 
      // unknown index may equal to other
      // integer index equal to another integer index if the integer values are the same
      virtual bool operator= (IndexSet & other);
      virtual ~IndexSet();
      virtual std::string toString();

    private:
      Index_type type;
  };

  
  // we reuse the ConstIndexSet if the value is the same
  class ConstIndexSet: public IndexSet
  {
    public:
      static ConstIndexSet* get_inst(size_t value);
      // only accept strict integer value expression, see bool SageInterface::isStrictIntegerType(SgType* t)
      static ConstIndexSet* get_inst(SgValueExp * v_exp);

      size_t getValue() {return value; };
      // we should not tweak the value once an instance is created. So there is no setValue()
      bool operator = (IndexSet & other); 
      std::string toString() {return "["+StringUtility::numberToString(value) + "]" ;};
    private:
      size_t value;
      ConstIndexSet(size_t i):IndexSet(Integer_type), value(i) {}
      static std::map <size_t, ConstIndexSet * >  constIndexMap;
  };

  // We only create at most one instance of this unknown indexset. It is a singleton.
  class UnknownIndexSet: public IndexSet
  {
    public:
      static UnknownIndexSet* get_inst();
      bool operator = (IndexSet & other) {return true; };  // may be equal to any others
      std::string toString() {return "[unknown]" ;};
    private:
      UnknownIndexSet (): IndexSet(Unknown_type) { }
      static UnknownIndexSet* inst; 
  };


  // The most intuitive implementation of array index vector
  class IndexVector_Impl : public IndexVector
  {
    public:
      size_t getSize() {  return index_vector.size(); };
      std::vector<IndexSet *> index_vector; // a vector of memory objects of named objects or temp expression objects
      std::string toString();
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


  // The connection to the ROSE AST, all concrete type, size , etc. information come from this side
  // -----------------------------------------------------------------------------------------------
  //
  // Three kinds of memory objects in ROSE AST: each of them can be one of the four categories above.
  // 1) SgExpression temporary variables: each SgExpression which is not named memory objects . They can be seen as compiler-generated discrete temp variables
  // 2) named memory objects : one object for each named variable 
  // 3) aliased memory objects: one object for each type

  class ExprObj // one object for each SgExpression which does not have a corresponding symbol
    // They are similar to compiler-generated temporaries for three operand AST format
  { 
    public:
      SgExpression* anchor_exp; 
      SgType* type; 
      // ObjSet* parent; // this field is not necessary, if this ExprObj is a filed of some aggregate types, this ExprObj should be NamedObj
      ExprObj (SgExpression* a, SgType* t): anchor_exp(a), type(t) {};
      SgType* getType() {return type;}
      // equal if and only the o2 is another ExprObj with the same SgExpression anchor
      bool operator == (ObjSet& o2) ;
      bool operator == (ExprObj& o2) ;
      bool isConstant () {return isSgValueExp(anchor_exp); } ; // if the expression object represent a constant value (SgValueExp)
      std::string toString();
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
      SgType* type;  // in most cases, the type here should be anchor_symbol->get_type(). But array element's type can be different from its array type
      ObjSet* parent; //exists for 1) compound variables like a.b, where a is b's parent, 2) also for array element, where array is the parent
      IndexVector*  array_index_vector; // exists for array element: the index vector of an array element. Ideally this data member could be reused for index of field of structure/class

      //Is this always true that the parent of a named object must be an expr object?
      NamedObj (SgSymbol* a, SgType* t, ObjSet* p, IndexVector* iv):anchor_symbol(a), type(t),parent(p), array_index_vector (iv){};
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
    public:
      ScalarExprObj (SgExpression* e, SgType* t):ExprObj(e,t) {}
      std::set<SgType*> getType();
      bool operator == (ObjSet& o2) ;
      std::string toString();
  };

  class LabeledAggregateExprObj: public LabeledAggregate_Impl, public ExprObj
  {
    public:
      LabeledAggregateExprObj (SgExpression* s, SgType* t);
      std::set<SgType*> getType();

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      // TODO bool operator == (const LabeledAggregate& that) const;
      //Total order relations (implemented by interface)
      //bool operator < (const LabeledAggregate& that) const;
      std::string toString();
  };

  class ArrayExprObj: public Array_Impl, public ExprObj
  {};

  class PointerExprObj: public Pointer_Impl, public ExprObj
  {
    public:
      PointerExprObj (SgExpression* e, SgType* t): ExprObj (e,t) {}
      std::set<SgType*> getType();
      // used for a pointer to non-array
      ObjSet* getDereference () ;
      // used for a pointer to an array
      ObjSet * getElements();
      // Returns true if this pointer refers to the same abstract object as that pointer.
      bool equalPoints(Pointer & that);

      // Returns true if this object and that object may/must refer to the same pointer memory object.
      bool operator == (ObjSet& o2) ;
      std::string toString();

  };

  // named object --------------------------------
  //
  class ScalarNamedObj: public Scalar_Impl, public NamedObj 
  {
    public:
      ScalarNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv): NamedObj (s,t,p, iv) {}
      std::set<SgType*> getType();
      bool operator == (ObjSet& o2) ;
      std::string toString();
  };

  class LabeledAggregateNamedObj: public LabeledAggregate_Impl, public NamedObj
  {
    public:
      LabeledAggregateNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv);
      std::set<SgType*> getType();

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      // TODO bool operator == (const LabeledAggregate& that) const;
      //Total order relations (implemented by interface)
      //bool operator < (const LabeledAggregate& that) const;

      std::string toString();
  };

  class ArrayNamedObj: public Array_Impl, public NamedObj
  {
    public:
      ArrayNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv);
      std::set <SgType*> getType();
      std::string toString();

      // Returns a memory object that corresponds to all the elements in the given array
      ObjSet* getElements() {return this; } ; 
      // Returns the memory object that corresponds to the elements described by the given abstract index, 
      ObjSet* getElements(IndexVector* ai);

      // number of dimensions of the array
      virtual size_t getNumDims();
     //TODO 
      bool operator == (const Array & that) const;
      bool operator < (const Array & that) const;
 
  };

  class PointerNamedObj: public Pointer_Impl, public NamedObj
  {
    public:
      PointerNamedObj   (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv): NamedObj (s,t,p, iv) {}
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

  // lower level, internal  builder for different objects -----------------------
  // Users should use ObjSet* ObjSetFactory::createObjSet (SgNode* n) instead
  
  // Create an aliased obj set from a type. It can return NULL since not all types are supported.
  ObjSet* createAliasedObjSet(SgType*t);  // One object per type, Type based alias analysis
  ObjSet* createNamedObjSet(SgSymbol* anchor_symbol, SgType* t, ObjSet* parent, IndexVector* iv); // any 
  ObjSet* createNamedOrAliasedObjSet(SgVarRefExp* r); // create NamedObjSet or AliasedObjSet (for pointer type) from a variable reference 
  ObjSet* createNamedObjSet(SgPntrArrRefExp* r); // create NamedObjSet from an array element access
  ObjSet* createExpressionObjSet(SgExpression* anchor_exp, SgType*t); 
  // ObjSet* createObjSet(SgNode*); // top level catch all case, declared in memory_object.h

  // Helper functions 
  // --------------------------------------
  // debugging
  void dump_aliased_objset_map (); 

  // A helper function to decide if two types are aliased
  // two cases: 1 they are the same type
  //            2 they have overlap (one type is a subtype of the other)
  bool isAliased (SgType * t1, SgType * t2 ); 

  // a helper function to check if a symbol is corresponding to a member variable declaration within SgClassDefinition or not
  bool isMemberVariableDeclarationSymbol(SgSymbol * s);

  // a helper function to fill up elements of ObjSet p from a class/structure type
  void fillUpElements (ObjSet* p, std::vector<LabeledAggregateField*> & elements, SgClassType* c_t);

  // convert std::vector<SgExpression*>* subscripts to IndexVector*  array_index_vector
  IndexVector * generateIndexVector (std::vector<SgExpression*>& subscripts); 

} // end namespace

#endif
