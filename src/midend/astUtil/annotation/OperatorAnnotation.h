
#ifndef OPERATOR_ANNOTATION_H
#define OPERATOR_ANNOTATION_H

#include "AnnotCollect.h"
#include "AnalysisInterface.h"
#include "AstInterface.h"
#include <iostream>
#include <list>

template <class Descriptor>
class OperatorAnnotCollection
: public AnnotCollectionBase<OperatorDeclaration>,
  public TypeCollection<Descriptor>
{
 protected:
  TypeCollection<Descriptor>::typemap;
 public:
  void read_descriptor( const OperatorDeclaration& target,
                        const std::string& annot, std::istream& in)
  {
     Descriptor d;
     d.read(in, target);
     add_annot( target, d);
  }
  void add_annot( const OperatorDeclaration& op, const Descriptor& d)
    {
      std::string sig = op.get_signiture();
      this->typemap[ sig ] = d;
    }
  bool known_operator( AstInterface& fa, 
                       const AstNodePtr& exp, AstInterface::AstNodeList* argp= 0, 
                       Descriptor* desc= 0, bool replpar = false,
                       Map2Object<AstInterface*, AstNodePtr, AstNodePtr>* astcodegen =0) const
  {
    AstInterface::AstNodeList args;
    AstInterface::AstTypeList params;
    AstNodePtr f;
    std::string fname;
    // operator cases:
    //  Is a function call and the grabbed f is a variable reference (functionRefExp), OR
    //  function definition 
    if (!(fa.IsFunctionCall(exp,&f, &args, 0, &params) && fa.IsVarRef(f,0,&fname))
       && !fa.IsFunctionDefinition(exp,&fname,&args,0,0, &params))
        return false;
    std::string sig = OperatorDeclaration::get_signiture(fa, fname, params);
    if (known_type( sig, desc)) {
       if (argp != 0)
          *argp = args;
       if (desc != 0 && replpar) {
         ParamDescriptor params = desc->get_param_decl().get_params();
         ReplaceParams repl( params, args, astcodegen);
         repl.add( "result", exp, astcodegen);
         desc->replace_val( repl);
       }
       return true;
    }
    return false;
  }
};

class OperatorInlineAnnotation
: public OperatorAnnotCollection<OperatorInlineDescriptor>
{
  virtual bool read_annot_name( const std::string& annotName) const
    { return annotName == "inline"; }
  static OperatorInlineAnnotation* inst;
  OperatorInlineAnnotation() {}
 public:
  static OperatorInlineAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorInlineAnnotation();
      return inst; }
  void Dump() const
    {
      std::cerr << "inline: \n";
      OperatorAnnotCollection<OperatorInlineDescriptor>::Dump();
    }
  bool known_operator( AstInterface& fa, const AstNodePtr& exp, SymbolicVal* val = 0) const;
  void register_annot()
    { ReadAnnotation::get_inst()->add_OperatorCollection(this); }
  //Check if an operator has a 'inline {this.operator_2(dim)' record. 
  //If yes, store the semantically equivalent operator into val
  bool get_inline( AstInterface& fa, const AstNodePtr& h, SymbolicVal* val = 0);
};

class OperatorModInfoCollection 
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "modify"; }
 public:
  void Dump() const
    { 
      std::cerr << "modify: \n"; 
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump(); 
    }
};
class OperatorReadInfoCollection
  : public OperatorAnnotCollection<OperatorSideEffectDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "read"; }
 public:
  void Dump() const
    {
      std::cerr << "read: \n";
      OperatorAnnotCollection<OperatorSideEffectDescriptor>::Dump();
    }
};
class OperatorSideEffectAnnotation : public FunctionSideEffectInterface
{
  OperatorModInfoCollection modInfo;
  OperatorReadInfoCollection readInfo;
  static OperatorSideEffectAnnotation* inst;
  OperatorSideEffectAnnotation() {}
 public:
  static OperatorSideEffectAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorSideEffectAnnotation();
      return inst; }
  void register_annot()
    {
      ReadAnnotation* op = ReadAnnotation::get_inst();
      op->add_OperatorCollection(&modInfo);
      op->add_OperatorCollection(&readInfo);
    }
  bool get_modify( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0);
  bool get_read( AstInterface& fa, const AstNodePtr& fc, CollectObject<AstNodePtr>* collect = 0);
  void Dump() const
    { modInfo.Dump(); readInfo.Dump(); }
};

class OperatorAliasCollection 
: public OperatorAnnotCollection <OperatorAliasDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "alias"; }
 public:
  void Dump() const
    { std::cerr << "alias: \n"; 
      OperatorAnnotCollection<OperatorAliasDescriptor>::Dump(); }
};
class OperatorAllowAliasCollection
: public OperatorAnnotCollection <OperatorAliasDescriptor>
{
  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "allow_alias"; }
 public:
  void Dump() const
    { std::cerr << "allow_alias: \n";
      OperatorAnnotCollection<OperatorAliasDescriptor>::Dump(); }
};

class OperatorAliasAnnotation : public FunctionAliasInterface
{
  OperatorAliasCollection aliasInfo;
  OperatorAllowAliasCollection allowaliasInfo;

  virtual bool read_annot_name( const std::string& annotname) const
   { return annotname == "alias" || annotname == "allow_alias"; }
  static OperatorAliasAnnotation* inst;
  OperatorAliasAnnotation() {}
 public:
  static OperatorAliasAnnotation* get_inst() 
    { if (inst == 0) inst = new OperatorAliasAnnotation();
      return inst; }
  //! Inherited from FunctionAliasInterface    
  bool may_alias(AstInterface& fa, const AstNodePtr& fc, const AstNodePtr& result, 
               CollectObject<std::pair<AstNodePtr,int> >& collectalias);
  //! Inherited from FunctionAliasInterface
  bool allow_alias( AstInterface& fa, const AstNodePtr& fd, 
                    CollectObject<std::pair<AstNodePtr,int> >& collectalias);
  void Dump() const
    { aliasInfo.Dump(); allowaliasInfo.Dump(); }
  void register_annot()
    { ReadAnnotation* op = ReadAnnotation::get_inst();
      op->add_OperatorCollection(&aliasInfo); 
      op->add_OperatorCollection(&allowaliasInfo); }
};
//! Replace an operator with another equivalent operation, which is specified by "inline" annotation
//e.g: operator floatArray::operator() (int index) {  inline { this.elem(index) }; }
//     All ..array(index).. will be replaced by array.elem(index) in the code
class OperatorInlineRewrite : public TransformAstTree
{
 public:
  virtual bool operator()( AstInterface& fa, const AstNodePtr& n,
                           AstNodePtr& result) 
  {
    SymbolicVal r;
    if (OperatorInlineAnnotation::get_inst()->get_inline( fa, n, &r)) {
       result = r.CodeGen(fa);
       return true;
    }
    return false;
  }
  AstNodePtr operator() ( AstInterface& fa, const AstNodePtr& r) 
   {
     return TransformAstTraverse( fa, r, *this);
   }
};

#endif
