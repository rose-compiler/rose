#ifndef memory_object_impl_INCLUDED
#define memory_object_impl_INCLUDED

#include "memory_object.h"

namespace AbstractMemoryObject 
{
  // One simple implementation for abstract memory object
  // This implementation is tied to the ROSE AST (another set of base classes such as expression,
  // named, and aliased objects)
  //
  // Big picture of this implementation
  // How the multiple inheritance results in the classes here
  //
  //                      Scalar            LabeledAggregate        Array       Pointer  Function
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
      // We implement operator < () at this level
      bool operator < (const ObjSet& other) const ;
  };

  class Function_Impl : public Function
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
      // We implement operator < () at this level
      bool operator < (const ObjSet& other) const ;
  };

  class LabeledAggregate_Impl : public LabeledAggregate
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
      bool operator < (const ObjSet& other) const ;

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
      bool operator < (const ObjSet& other) const ;

 };

  class Pointer_Impl: public Pointer 
  {
    public:
      virtual bool maySet();
      virtual bool mustSet();
      virtual size_t objCount();    
      bool operator < (const ObjSet& other) const ;
  };

  //! A set of index values, could only have constant integer values or unknown values in this implementation.
  class IndexSet
  {
    public:
      enum Index_type {
        Integer_type = 0,
        Unknown_type 
      };

      Index_type getType() const {return type; };

      size_t getSize() {return 1;}; // Simple thing first

      IndexSet (Index_type t):type(t){} 
      virtual bool operator== (const IndexSet & other) const;
      virtual bool mayEqual (const IndexSet & other)  const ;
      virtual bool mustEqual (const IndexSet & other) const ;
      virtual bool operator!= (const IndexSet & other) const;
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
      // integer index equal to another integer index if the integer values are the same
      bool operator== (const IndexSet & other) const; 
      bool mayEqual (const IndexSet & other) const; 
      bool mustEqual (const IndexSet & other) const; 
      bool operator != (const IndexSet & other) const; 
      std::string toString() {return "["+Rose::StringUtility::numberToString(value) + "]" ;};
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
      // unknown index may equal to other
      bool operator== (const IndexSet & /*other*/) const { return true; };  // may be equal to any others
      bool mayEqual (const IndexSet & /*other*/) const { return true; };
      bool mustEqual (const IndexSet & /*other*/) const { return false; };

      bool operator != (const IndexSet & /*other*/) const { return true; };
      std::string toString() {return "[unknown]" ;};
    private:
      UnknownIndexSet (): IndexSet(Unknown_type) { }
      static UnknownIndexSet* inst; 
  };


  // The most intuitive implementation of array index vector
  class IndexVector_Impl : public IndexVector
  {
    public:
      size_t getSize() const {  return index_vector.size(); };
      std::vector<IndexSet *> index_vector; // a vector of memory objects of named objects or temp expression objects
      std::string toString();
      bool operator== (const IndexVector & other) const ;
      bool mayEqual (const IndexVector & other) const ;
      bool mustEqual (const IndexVector & other) const ;

      bool operator != (const IndexVector & other) const ;
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
  // 1) SgExpression temporary variables: each SgExpression which is not named memory objects. 
  //                         They can be seen as compiler-generated discrete temp variables
  // 2) named memory objects : one object for each named variable 
  // 3) aliased memory objects: one object for each type, used for a vague memory object
  //                             representing a set of aliased objects. All aliased objects of the
  //                             same type are represented by a single aliased memory object

  class ExprObj // one object for each SgExpression which does not have a corresponding symbol
    // They are similar to compiler-generated temporaries for three operand AST format
  { 
    public:
      SgExpression* anchor_exp; 
      SgType* type; 
      // ObjSet* parent; // this field is not necessary, if this ExprObj is a filed of some aggregate types, this ExprObj should be NamedObj
      ExprObj (SgExpression* a, SgType* t): anchor_exp(a), type(t) {};
      SgType* getType() const {return type;}
      // equal if and only the o2 is another ExprObj with the same SgExpression anchor
      bool operator== (const ExprObj& o2) const;
      // this is necessary to allow derived classes to implement ObjSet::operator==(ObjSet&)
      bool operator== (const ObjSet& o2) const; 
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      bool isConstant () {return isSgValueExp(anchor_exp); } ; // if the expression object represent a constant value (SgValueExp)
      std::string toString();
  };

  // Correspond to variables that are explicit named in the source code
  // Including: local, global, and static variables, as well as their fields
  // Named objects may not directly alias each other since they must be stored disjointly
  // In most cases, two named objects are equal to each other if they correspond to the same entry in the application's symbol table. 
  //
  // However, for a symbol representing a field of a structure, it can be shared by many instances
  // of the structure. In this case, a parent objSet is needed to distinguish between them 
  class NamedObj  // one object for each named variable with a symbol
  { 
    public:
      SgSymbol* anchor_symbol; // could be a symbol for variable, function, class/structure, etc.
      SgType* type;  // in most cases, the type here should be anchor_symbol->get_type(). But array element's type can be different from its array type
      ObjSet* parent; //exists for 1) compound variables like a.b, where a is b's parent, 2) also for array element, where array is the parent
      IndexVector*  array_index_vector; // exists for array element: the index vector of an array element. Ideally this data member could be reused for index of field of structure/class

      //Is this always true that the parent of a named object must be an expr object?
      NamedObj (SgSymbol* a, SgType* t, ObjSet* p, IndexVector* iv):anchor_symbol(a), type(t),parent(p), array_index_vector (iv){};
      SgType* getType() const {return type;}
      ObjSet* getParent() {return parent; } 
      SgSymbol* getSymbol() {return anchor_symbol;}

      std::string getName() {return anchor_symbol->get_name().getString(); }
      std::string toString(); 

      bool operator== (const NamedObj& o2) const;
      bool mayEqual (const NamedObj& o2) const; 
      bool mustEqual (const NamedObj& o2) const; 

      bool operator== (const ObjSet& o2) const;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
  };

  //  Memory regions that may be accessible via a pointer, such as heap memory
  //  This implementation does not track accurate aliases, an aliased memory object and 
  //  an aliased or named object may be equal if they have the same type
  class AliasedObj 
  {  // One object for each type
    public: 
      SgType* type; 
      AliasedObj (SgType* t): type(t) {};
      SgType* getType() const {return type;}
      std::string toString(); 

      bool operator == (const AliasedObj & o2) const;
      bool mayEqual (const AliasedObj & o2) const;
      bool mustEqual (const AliasedObj & o2) const;

      bool operator == (const ObjSet& o2) const;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
  };

  //Derived classes for each kind of each category
  // expression object ------------------------------
  class ScalarExprObj: public Scalar_Impl, public ExprObj
  {
    public:
      ScalarExprObj (SgExpression* e, SgType* t):ExprObj(e,t) {}
      std::set<SgType*> getType() ;
      // Implement ObjSet::operator== () at this level, through ExprObj::operator==().
      bool operator == (const ObjSet& o2) const;
      // multiple inheritance, must be clear about the behavior inherited from both parents
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

   // Does it make sense to have expressions which are of a function type?
   //  I guess so, like function passed as parameter, or a  pointer to a function?
   class FunctionExprObj: public Function_Impl, public ExprObj
  {
    public:
      FunctionExprObj (SgExpression* e, SgType* t):ExprObj(e,t) {}
      std::set<SgType*> getType() ;
      // Implement ObjSet::operator== () at this level, through ExprObj::operator==().
      bool operator == (const ObjSet& o2) const;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 

      std::string toString();
  };

  class LabeledAggregateExprObj: public LabeledAggregate_Impl, public ExprObj
  {
    public:
      LabeledAggregateExprObj (SgExpression* s, SgType* t);
      std::set<SgType*> getType();

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      bool operator == (const ObjSet& o2) const;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class ArrayExprObj: public Array_Impl, public ExprObj
  {
    public:
      ArrayExprObj(SgExpression* e, SgType* t): ExprObj (e,t) {}
      std::set<SgType*> getType();
      //TODO other member functions still make sense?
      bool operator == (const ObjSet& o2) const;
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

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
      bool operator == (const ObjSet& o2) const;
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();

  };

  // named object --------------------------------
  //
  class ScalarNamedObj: public Scalar_Impl, public NamedObj 
  {
    public:
      ScalarNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv): NamedObj (s,t,p, iv) {}
      std::set<SgType*> getType();
      bool operator == (const ObjSet & that) const ;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class FunctionNamedObj: public Function_Impl, public NamedObj 
  {
    public:

      // simple constructor, a function symbol is enough
      FunctionNamedObj (SgSymbol* s): NamedObj (s,s->get_type(), nullptr, nullptr) {}
      // I am not sure when a function can be used as a child and an array element. But this is
      // provided just in case
      FunctionNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv): NamedObj (s,t,p, iv) {}
      std::set<SgType*> getType();
      bool operator == (const ObjSet & that) const ;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class LabeledAggregateNamedObj: public LabeledAggregate_Impl, public NamedObj
  {
    public:
      LabeledAggregateNamedObj (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv);
      std::set<SgType*> getType();

      // Returns true if this object and that object may/must refer to the same labeledAggregate memory object.
      bool operator == (const ObjSet & that) const ;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
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
      ObjSet* getElements(IndexVector* ai) ;

      // number of dimensions of the array
      virtual size_t getNumDims();

  // rare case that an array is dereferenced, treated as array[0]
      ObjSet* getDereference () ;
      bool operator == (const ObjSet & that) const ;
 
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
  };

  class PointerNamedObj: public Pointer_Impl, public NamedObj
  {
    public:
      PointerNamedObj   (SgSymbol* s, SgType* t, ObjSet* p, IndexVector* iv): NamedObj (s,t,p, iv) {}
      std::set<SgType*> getType() ;
      // used for a pointer to non-array
      ObjSet* getDereference () ;
      // used for a pointer to an array
      ObjSet * getElements() ;
      // Returns true if this pointer refers to the same abstract object as that pointer.
      bool equalPoints(Pointer & that);
      // Returns true if this object and that object may/must refer to the same pointer memory object.
      bool operator == (const ObjSet & that) const ;

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  // aliased object -----------------------------
  class ScalarAliasedObj: public Scalar_Impl, public AliasedObj
  {
    public:
      ScalarAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      bool operator == (const ObjSet& o2) const; 

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();

  };

  class FunctionAliasedObj: public Function_Impl, public AliasedObj
  {
    public:
      FunctionAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      bool operator == (const ObjSet& o2) const; 

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class LabeledAggregateAliasedObj : public  LabeledAggregate_Impl, public AliasedObj
  {
    public:
      LabeledAggregateAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      bool operator == (const ObjSet& o2) const; 
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class ArrayAliasedObj: public Array_Impl, public AliasedObj
  {
    public:
      ArrayAliasedObj (SgType* t): AliasedObj(t){}
      std::set<SgType*> getType();
      bool operator == (const ObjSet& o2) const; 
      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
      std::string toString();
  };

  class PointerAliasedObj: public Pointer_Impl, public AliasedObj
  {
    public:
      PointerAliasedObj (SgType* t): AliasedObj(t){}
      ObjSet* getDereference ();
      bool equalPoints(Pointer & that);
      std::set<SgType*> getType();
      bool operator == (const ObjSet& o2) const; 

      bool mayEqual (const ObjSet& o2) const; 
      bool mustEqual (const ObjSet& o2) const; 
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

  // Helper functions 
  // --------------------------------------
  // debugging
  void dump_aliased_objset_map (); 

  // A helper function to decide if two types are aliased
  // two cases: 1 they are the same type
  //            2 they have overlap (one type is a subtype of the other)
  bool isAliased (const SgType * t1, const SgType * t2 ); 

  // a helper function to check if a symbol is corresponding to a member variable declaration within SgClassDefinition or not
  bool isMemberVariableDeclarationSymbol(SgSymbol * s);

  // a helper function to fill up elements of ObjSet p from a class/structure type
  void fillUpElements (ObjSet* p, std::vector<LabeledAggregateField*> & elements, SgClassType* c_t);

  // convert std::vector<SgExpression*>* subscripts to IndexVector*  array_index_vector
  IndexVector * generateIndexVector (std::vector<SgExpression*>& subscripts); 

} // end namespace

#endif
