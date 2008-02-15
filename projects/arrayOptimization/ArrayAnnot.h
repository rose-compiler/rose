#ifndef ARRAY_ANNOT_H
#define ARRAY_ANNOT_H

#include <AnnotExpr.h>
#include <AnnotCollect.h>
#include <OperatorDescriptors.h>
#include <CPPAnnotation.h>
#include <ValuePropagate.h>
#include <list>


class ArrayShapeDescriptor {
  SymbolicValDescriptor dimension;
  SymbolicFunctionDeclarationGroup length;
 public:
  void push_back( const ReadSymbolicFunctionDeclaration& cur);
  void write(STD ostream& out) const;
  void Dump() const;
  bool read( STD istream& in);

  const SymbolicValDescriptor& get_dimension() const { return dimension; }
  SymbolicValDescriptor& get_dimension() { return dimension; }
  bool get_dimension( int& val) const ;

  void set_length( const SymbolicFunctionDeclarationGroup v) { length = v; }
  SymbolicFunctionDeclarationGroup get_length() const { return length; }
  bool get_length( int dim, SymbolicVal& result) const 
    {
       STD vector<SymbolicVal> args;
       args.push_back( SymbolicConst(dim));
       return length.get_val(args, result);
    }

  void replace_var(  const STD string& varname, const SymbolicVal& repl)
   { 
     dimension.replace_var( varname, repl);
     length.replace_var(varname, repl);
   }
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
   { 
     dimension.replace_val(repl);
     length.replace_val(repl);
   }
};

class ArrayElemDescriptor 
{
  SymbolicFunctionDeclarationGroup elem;
 public:
  void push_back( const ReadSymbolicFunctionDeclaration& cur);
  void Dump() const;
  void write(STD ostream& out) const;
  bool read(STD istream& in);
  void replace_var(  const STD string& varname, const SymbolicVal& repl)
   { 
     elem.replace_var( varname, repl);
   }
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
   { 
     elem.replace_val(repl);
   }

  const SymbolicFunctionDeclarationGroup& get_elem()  const { return elem; }
};

class ArrayDescriptor 
: public ArrayShapeDescriptor, public ArrayElemDescriptor
{
 public:
  void push_back( const ReadSymbolicFunctionDeclaration& cur);
  bool read( STD istream& in) ;
  void write(STD ostream& out) const;
  void Dump() const;
  void replace_var(  const STD string& varname, const SymbolicVal& repl)
   { 
     ArrayShapeDescriptor::replace_var( varname, repl);
     ArrayElemDescriptor::replace_var( varname, repl);
   }
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
   { 
     ArrayShapeDescriptor :: replace_val(repl);
     ArrayElemDescriptor :: replace_val(repl);
   }
};

class ArrayDefineDescriptor : public ArrayDescriptor
{
  SymbolicFunctionDeclarationGroup reshape;
 public:
  void push_back( const ReadSymbolicFunctionDeclaration& cur);
  bool read( STD istream& in) ;
  void write(STD ostream& out) const;
  void Dump() const;
  void replace_var(  const STD string& varname, const SymbolicVal& repl);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);

  SymbolicFunctionDeclarationGroup get_reshape() const 
    {
      return reshape;
    }
  
};

class ArrayOptDescriptor : public ArrayDescriptor
{
  typedef ContainerDescriptor <STD list<DefineVariableDescriptor>, 
                               DefineVariableDescriptor, ';', '{', '}'> DefContainer;
  DefContainer defs;
 public:
  typedef STD list<DefineVariableDescriptor>::iterator InitVarIterator;
  
  InitVarIterator init_var_begin() 
    {
      return defs.begin();
    }
  InitVarIterator init_var_end()
    {
      return defs.end();
    }
  
      

  bool read( STD istream& in) ;
  void write(STD ostream& out) const;
  void Dump() const;
  void replace_var(  const STD string& varname, const SymbolicVal& repl);
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl);
};

class ArrayConstructDescriptor 
: public OPDescriptorTemp
          < CollectPair< ContainerDescriptor<STD list<SymbolicValDescriptor>,
                                             SymbolicValDescriptor, ',', '(', ')'>,
                         ArrayDescriptor, 0 > >
{
  typedef OPDescriptorTemp
          < CollectPair< ContainerDescriptor<STD list<SymbolicValDescriptor>, 
                                             SymbolicValDescriptor, ',', '(', ')'>,
                         ArrayDescriptor, 0 > 
          > BaseClass;
 public:
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
   {
     for (STD list<SymbolicValDescriptor>::iterator p = first.begin(); p != first.end(); ++p) { 
         (*p).replace_val(repl);
     }
     second.replace_val(repl);
   }
};

class ArrayModifyDescriptor : 
public OPDescriptorTemp < CollectPair< CloseDescriptor<SymbolicValDescriptor, '(', ')'>, 
                         ArrayDescriptor,0> > 
{
  typedef OPDescriptorTemp
    < CollectPair< CloseDescriptor<NameDescriptor, '(', ')'>, 
                         ArrayDescriptor,0> >  BaseClass;
 public:
  void replace_val(MapObject<SymbolicVal, SymbolicVal>& repl)
   {
     first.replace_val(repl);
     second.replace_val(repl);
   }
};

class ArrayCollection 
  : public TypeAnnotCollection< ArrayDefineDescriptor>,
    public CPPTypeCollection< ArrayDefineDescriptor>
{
   typedef TypeAnnotCollection< ArrayDefineDescriptor > BaseClass;
  virtual bool read_annot_name( const STD string& annotName) const 
    { return annotName == "array"; }
 public:
  ArrayCollection() : CPPTypeCollection<ArrayDefineDescriptor>(this) {}
  void Dump() const 
    { STD cerr << "arrays: \n"; BaseClass::Dump(); }
};

class ArrayOptCollection : public TypeAnnotCollection< ArrayOptDescriptor>
{
  typedef TypeAnnotCollection< ArrayOptDescriptor > BaseClass;
  virtual bool read_annot_name( const STD string& annotName) const 
    { return annotName == "array_optimize"; }
 public:
  void Dump() const 
    { STD cerr << "array optimizations: \n"; BaseClass::Dump(); }
};

class ArrayConstructOpCollection
: public OperatorAnnotCollection<ArrayConstructDescriptor>
{
  virtual bool read_annot_name( const STD string& annotName) const
    { return annotName == "construct_array"; }
 public:
  void Dump() const
    {
      STD cerr << "construct_array: \n";
      OperatorAnnotCollection<ArrayConstructDescriptor>::Dump();
    }
};

class ArrayModifyOpCollection : public OperatorAnnotCollection<ArrayModifyDescriptor>
{
  virtual bool read_annot_name( const STD string& annotName) const
    { return annotName == "modify_array"; }
 public:
  void Dump() const
    { 
      STD cerr << "modify_array: \n"; 
      OperatorAnnotCollection<ArrayModifyDescriptor>::Dump(); 
    }
};

class ArrayAnnotation 
    : public FunctionSideEffectInterface,
      public FunctionAliasInterface
{
  //map <STD string, OperatorDeclaration> decl;
  ArrayCollection arrays;
  ArrayOptCollection arrayopt;
  ArrayModifyOpCollection arrayModify;
  ArrayConstructOpCollection arrayConstruct;
  
  static ArrayAnnotation* inst;

  virtual bool may_alias(AstInterface& fa, const AstNodePtr& fc, 
                         const AstNodePtr& result,
                         CollectObject< STD pair<AstNodePtr, int> >& collectalias);
  virtual bool allow_alias(AstInterface& fa, const AstNodePtr& fc, 
                         CollectObject< STD pair<AstNodePtr, int> >& collectalias);
  virtual bool get_modify(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);
  virtual bool get_read(AstInterface& fa, const AstNodePtr& fc,
                               CollectObject<AstNodePtr>* collect = 0);
  ArrayAnnotation() {}
 public:
  static ArrayAnnotation* get_inst();

  void register_annot(); 
  void Dump() const;

  bool known_array( CPPAstInterface& fa, const AstNodePtr& array, ArrayDefineDescriptor* d = 0);
  bool known_array_type(CPPAstInterface& fa,  const AstNodeType& array, ArrayDefineDescriptor* d = 0);
  bool has_array_opt( CPPAstInterface& fa, const AstNodePtr array, ArrayOptDescriptor* d = 0);

  bool is_array_mod_op( CPPAstInterface& fa, const AstNodePtr& arrayExp,
                        AstNodePtr* modArray = 0, ArrayDescriptor* desc = 0, 
                        bool* reshapeArray = 0, ReplaceParams* repl = 0);
  bool is_array_construct_op( CPPAstInterface& fa, const AstNodePtr& arrayExp,
                              CPPAstInterface::AstNodeList* alias = 0,
                              ArrayDescriptor* desc = 0, ReplaceParams* repl = 0);

  bool is_access_array_elem( CPPAstInterface& fa, const AstNodePtr& orig,
                          AstNodePtr* array=0, CPPAstInterface::AstNodeList* args=0);
  bool is_access_array_length( CPPAstInterface& fa, const AstNodePtr& orig,
                            AstNodePtr* array=0, AstNodePtr* dimast = 0, int* dim =0);
  bool is_access_array_elem( CPPAstInterface& fa, const SymbolicVal& orig,
                    AstNodePtr* array=0, SymbolicFunction::Arguments* args=0);
  bool is_access_array_length( CPPAstInterface& fa, const SymbolicVal& orig, AstNodePtr* array=0, SymbolicVal *dim = 0);

  SymbolicVal create_access_array_elem( const AstNodePtr& array, 
                           const SymbolicFunction::Arguments& args);
  SymbolicVal create_access_array_length( const AstNodePtr& array, const SymbolicVal& dim);
  AstNodePtr create_access_array_elem( CPPAstInterface& fa, 
                                        const AstNodePtr& array,
				  const CPPAstInterface::AstNodeList& args);
  AstNodePtr create_access_array_length( CPPAstInterface& fa, const AstNodePtr& array, 
					 int dim);

  bool is_reshape_array( CPPAstInterface& fa, const AstNodePtr& orig,
			AstNodePtr* array=0, CPPAstInterface::AstNodeList* args=0);
  AstNodePtr create_reshape_array( CPPAstInterface& fa, const AstNodePtr& array,
				  const CPPAstInterface::AstNodeList& args);
};

#endif

